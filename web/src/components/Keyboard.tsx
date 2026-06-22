import { useState, useEffect, useCallback, useMemo } from 'react';
import { useSynthEngine } from '../audio/engineContext';

interface PianoKey {
  note: number;
  label: string;
  isBlack: boolean;
  keyboardKey: string; // 空文字なら PC キー割り当て無し
  // レイアウト用: 白鍵幅を1単位とした左端位置
  left: number;
}

// 基準ノート（C4）
const START_NOTE = 60;
// 1オクターブあたりの白鍵数
const WHITE_PER_OCTAVE = 7;

// 音名（C を0とした半音インデックス順）
const NOTE_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
// 黒鍵となる半音インデックス（C#, D#, F#, G#, A#）
const BLACK_SEMITONES = new Set([1, 3, 6, 8, 10]);

// オクターブ内の白鍵: 半音インデックス → 白鍵インデックス(0..6)
const WHITE_INDEX_IN_OCTAVE: Record<number, number> = {
  0: 0, 2: 1, 4: 2, 5: 3, 7: 4, 9: 5, 11: 6,
};
// オクターブ内の黒鍵の左端位置（白鍵幅を1単位、オクターブ先頭Cを0とする）。
// 既存レイアウトの値をそのまま踏襲している。
const BLACK_LEFT_IN_OCTAVE: Record<number, number> = {
  1: 0.65, // C#
  3: 1.65, // D#
  6: 3.65, // F#
  8: 4.65, // G#
  10: 5.65, // A#
};

const BLACK_KEY_WIDTH_RATIO = 0.55;

// 1オクターブ目（C4..C5）の PC キー割り当て。
// 2オクターブ表示時も、PC キーはこの下側オクターブのみを担当する。
const KEYBOARD_KEYS: Record<number, string> = {
  60: 'a', 61: 'w', 62: 's', 63: 'e', 64: 'd', 65: 'f',
  66: 't', 67: 'g', 68: 'y', 69: 'h', 70: 'u', 71: 'j', 72: 'k',
};

// PC キー → ノート番号（オクターブ数に依存しないため固定でよい）
const KEY_TO_NOTE = new Map(
  Object.entries(KEYBOARD_KEYS).map(([note, key]) => [key, Number(note)]),
);

// 2オクターブ表示に切り替える画面幅のしきい値（px）
const TWO_OCTAVE_BREAKPOINT_PX = 768;

// 画面幅から表示オクターブ数を決める
function octavesForWidth(width: number): number {
  return width >= TWO_OCTAVE_BREAKPOINT_PX ? 2 : 1;
}

// 指定オクターブ数ぶんの鍵盤を生成する（最後に+1鍵の C を足す）
function buildKeys(octaves: number): PianoKey[] {
  const keys: PianoKey[] = [];
  const semitoneCount = 12 * octaves;
  for (let i = 0; i <= semitoneCount; i++) {
    const note = START_NOTE + i;
    const oct = Math.floor(i / 12);
    const semi = i % 12;
    const isBlack = BLACK_SEMITONES.has(semi);
    const left =
      (isBlack ? BLACK_LEFT_IN_OCTAVE[semi] : WHITE_INDEX_IN_OCTAVE[semi]) +
      oct * WHITE_PER_OCTAVE;
    keys.push({
      note,
      label: `${NOTE_NAMES[semi]}${4 + oct}`,
      isBlack,
      keyboardKey: KEYBOARD_KEYS[note] ?? '',
      left,
    });
  }
  return keys;
}

export function Keyboard() {
  const engine = useSynthEngine();
  const [pressedNotes, setPressedNotes] = useState<Set<number>>(new Set());

  // 画面幅に応じて 1 / 2 オクターブを切り替える
  const [octaves, setOctaves] = useState(() => octavesForWidth(window.innerWidth));
  useEffect(() => {
    const onResize = () => setOctaves(octavesForWidth(window.innerWidth));
    window.addEventListener('resize', onResize);
    return () => window.removeEventListener('resize', onResize);
  }, []);

  const keys = useMemo(() => buildKeys(octaves), [octaves]);
  const whiteKeys = useMemo(() => keys.filter((k) => !k.isBlack), [keys]);
  const blackKeys = useMemo(() => keys.filter((k) => k.isBlack), [keys]);
  const whiteKeyCount = whiteKeys.length;

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

  // PCキーイベントの登録
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      // テキスト入力中はスキップ
      const target = e.target as HTMLElement;
      if (['INPUT', 'TEXTAREA', 'SELECT'].includes(target.tagName)) return;
      if (e.repeat) return;
      const note = KEY_TO_NOTE.get(e.key.toLowerCase());
      if (note !== undefined) noteOn(note);
    };

    const handleKeyUp = (e: KeyboardEvent) => {
      const note = KEY_TO_NOTE.get(e.key.toLowerCase());
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
  const wPct = 100 / whiteKeyCount;

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
        {whiteKeys.map((key) => (
          <div
            key={key.note}
            onMouseDown={() => noteOn(key.note)}
            onMouseUp={() => noteOff(key.note)}
            onMouseLeave={() => {
              if (pressedNotes.has(key.note)) noteOff(key.note);
            }}
            style={{
              position: 'absolute',
              left: `${key.left * wPct}%`,
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
        {blackKeys.map((key) => (
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
              left: `${key.left * wPct}%`,
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
