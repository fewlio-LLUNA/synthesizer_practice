import { useState, useEffect, useCallback } from 'react';
import { useSynthEngine } from '../audio/engineContext';

interface PianoKey {
  note: number;
  label: string;
  isBlack: boolean;
  keyboardKey: string;
}

const PIANO_KEYS: PianoKey[] = [
  { note: 60, label: 'C4', isBlack: false, keyboardKey: 'a' },
  { note: 61, label: 'C#4', isBlack: true, keyboardKey: 'w' },
  { note: 62, label: 'D4', isBlack: false, keyboardKey: 's' },
  { note: 63, label: 'D#4', isBlack: true, keyboardKey: 'e' },
  { note: 64, label: 'E4', isBlack: false, keyboardKey: 'd' },
  { note: 65, label: 'F4', isBlack: false, keyboardKey: 'f' },
  { note: 66, label: 'F#4', isBlack: true, keyboardKey: 't' },
  { note: 67, label: 'G4', isBlack: false, keyboardKey: 'g' },
  { note: 68, label: 'G#4', isBlack: true, keyboardKey: 'y' },
  { note: 69, label: 'A4', isBlack: false, keyboardKey: 'h' },
  { note: 70, label: 'A#4', isBlack: true, keyboardKey: 'u' },
  { note: 71, label: 'B4', isBlack: false, keyboardKey: 'j' },
  { note: 72, label: 'C5', isBlack: false, keyboardKey: 'k' },
];

const WHITE_KEYS = PIANO_KEYS.filter((k) => !k.isBlack);
const BLACK_KEYS = PIANO_KEYS.filter((k) => k.isBlack);
const WHITE_KEY_COUNT = WHITE_KEYS.length; // 8

// 黒鍵の左端位置（白鍵幅を1単位とした倍率）
const BLACK_KEY_LEFT: Record<number, number> = {
  61: 0.65, // C#4
  63: 1.65, // D#4
  66: 3.65, // F#4
  68: 4.65, // G#4
  70: 5.65, // A#4
};

const BLACK_KEY_WIDTH_RATIO = 0.55;

export function Keyboard() {
  const engine = useSynthEngine();
  const [pressedNotes, setPressedNotes] = useState<Set<number>>(new Set());

  const noteOn = useCallback(
    (note: number) => {
      if (!engine) return;
      engine.noteOn(note);
      setPressedNotes((prev) => new Set(prev).add(note));
    },
    [engine]
  );

  const noteOff = useCallback(
    (note: number) => {
      if (!engine) return;
      engine.noteOff(note);
      setPressedNotes((prev) => {
        const next = new Set(prev);
        next.delete(note);
        return next;
      });
    },
    [engine]
  );

  // PCキーボードイベントの登録
  useEffect(() => {
    const keyToNote = new Map(PIANO_KEYS.map((k) => [k.keyboardKey, k.note]));

    const handleKeyDown = (e: KeyboardEvent) => {
      // テキスト入力中はスキップ
      const target = e.target as HTMLElement;
      if (['INPUT', 'TEXTAREA', 'SELECT'].includes(target.tagName)) return;
      if (e.repeat) return;
      const note = keyToNote.get(e.key.toLowerCase());
      if (note !== undefined) noteOn(note);
    };

    const handleKeyUp = (e: KeyboardEvent) => {
      const note = keyToNote.get(e.key.toLowerCase());
      if (note !== undefined) noteOff(note);
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);
    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, [noteOn, noteOff]);

  // 各白鍵の幅（コンテナ幅に対する%）
  const wPct = 100 / WHITE_KEY_COUNT;

  return (
    <div
      style={{
        background: 'var(--color-panel)',
        border: '1px solid var(--color-panel-border)',
        borderRadius: '4px',
        padding: '12px 16px',
      }}
    >
      <div
        style={{
          fontSize: '11px',
          color: 'var(--color-text-dim)',
          marginBottom: '8px',
        }}
      >
        鍵盤 — クリックまたはキーボード（白鍵: A S D F G H J K / 黒鍵: W E T Y U）
      </div>
      <div
        style={{
          position: 'relative',
          height: '120px',
          userSelect: 'none',
        }}
      >
        {/* 白鍵 */}
        {WHITE_KEYS.map((key, i) => (
          <div
            key={key.note}
            onMouseDown={() => noteOn(key.note)}
            onMouseUp={() => noteOff(key.note)}
            onMouseLeave={() => {
              if (pressedNotes.has(key.note)) noteOff(key.note);
            }}
            style={{
              position: 'absolute',
              left: `${i * wPct}%`,
              width: `${wPct - 0.3}%`,
              top: 0,
              bottom: 0,
              background: pressedNotes.has(key.note)
                ? 'var(--color-accent)'
                : '#d8d8d8',
              border: '1px solid #666',
              borderRadius: '0 0 4px 4px',
              cursor: 'pointer',
              zIndex: 1,
              display: 'flex',
              alignItems: 'flex-end',
              justifyContent: 'center',
              paddingBottom: '6px',
              fontSize: '10px',
              color: pressedNotes.has(key.note) ? '#fff' : '#555',
              boxSizing: 'border-box',
              transition: 'background 0.04s',
            }}
          >
            {key.keyboardKey.toUpperCase()}
          </div>
        ))}
        {/* 黒鍵 */}
        {BLACK_KEYS.map((key) => (
          <div
            key={key.note}
            onMouseDown={(e) => {
              e.stopPropagation();
              noteOn(key.note);
            }}
            onMouseUp={(e) => {
              e.stopPropagation();
              noteOff(key.note);
            }}
            onMouseLeave={() => {
              if (pressedNotes.has(key.note)) noteOff(key.note);
            }}
            style={{
              position: 'absolute',
              left: `${BLACK_KEY_LEFT[key.note] * wPct}%`,
              width: `${BLACK_KEY_WIDTH_RATIO * wPct - 0.3}%`,
              top: 0,
              height: '62%',
              background: pressedNotes.has(key.note)
                ? 'var(--color-accent)'
                : '#1a1a1a',
              border: '1px solid #000',
              borderRadius: '0 0 3px 3px',
              cursor: 'pointer',
              zIndex: 2,
              boxSizing: 'border-box',
              transition: 'background 0.04s',
            }}
          />
        ))}
      </div>
    </div>
  );
}
