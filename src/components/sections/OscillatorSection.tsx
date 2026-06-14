import { useState } from 'react';
import type { SectionProps, WaveformType, OscillatorParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_OSCILLATOR } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialParams?: OscillatorParams;
}

const WAVEFORMS: { value: WaveformType; label: string; paramId: string }[] = [
  { value: 'sine', label: 'Sin', paramId: 'oscillator.waveform.sine' },
  { value: 'sawtooth', label: 'Saw', paramId: 'oscillator.waveform.sawtooth' },
  { value: 'square', label: 'Sqr', paramId: 'oscillator.waveform.square' },
  { value: 'triangle', label: 'Tri', paramId: 'oscillator.waveform.triangle' },
];

const OCTAVES: number[] = [-2, -1, 0, 1, 2];

export function OscillatorSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_OSCILLATOR;
  const [waveform, setWaveform] = useState<WaveformType>(init.waveform);
  const [detune, setDetune] = useState(init.detune);
  const [octave, setOctave] = useState(init.octave);
  const [level, setLevel] = useState(init.level);

  const handleWaveformChange = (value: WaveformType) => {
    setWaveform(value);
    engine?.setOscillatorParams({ waveform: value });
  };

  const handleDetuneChange = (value: number) => {
    setDetune(value);
    engine?.setOscillatorParams({ detune: value });
  };

  const handleOctaveChange = (value: number) => {
    setOctave(value);
    engine?.setOscillatorParams({ octave: value });
  };

  const handleLevelChange = (value: number) => {
    setLevel(value);
    engine?.setOscillatorParams({ level: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Oscillator</h3>
      <div
        style={{ marginBottom: '10px' }}
        onMouseEnter={() => onParamHover('oscillator.waveform')}
        onMouseLeave={() => onParamHover(null)}
      >
        <div style={rowLabelStyle}>Waveform</div>
        <div style={{ display: 'flex', gap: '6px', flexWrap: 'wrap' }}>
          {WAVEFORMS.map(({ value, label, paramId }) => (
            <label
              key={value}
              style={radioLabelStyle}
              onMouseEnter={(e) => {
                e.stopPropagation();
                onParamHover(paramId);
              }}
              onMouseLeave={(e) => {
                e.stopPropagation();
                onParamHover('oscillator.waveform');
              }}
            >
              <input
                type="radio"
                name="osc-waveform"
                value={value}
                checked={waveform === value}
                onChange={() => handleWaveformChange(value)}
                style={{ marginRight: '3px' }}
              />
              {label}
            </label>
          ))}
        </div>
      </div>
      <div
        style={{ marginBottom: '10px' }}
        onMouseEnter={() => onParamHover('oscillator.octave')}
        onMouseLeave={() => onParamHover(null)}
      >
        <div style={rowLabelStyle}>Octave</div>
        <div style={{ display: 'flex', gap: '4px' }}>
          {OCTAVES.map((value) => (
            <button
              key={value}
              type="button"
              onClick={() => handleOctaveChange(value)}
              style={{
                ...octaveButtonStyle,
                background:
                  octave === value
                    ? 'var(--color-accent)'
                    : 'var(--color-knob)',
                color: octave === value ? '#1a1a1a' : 'var(--color-text)',
              }}
            >
              {value > 0 ? `+${value}` : value}
            </button>
          ))}
        </div>
      </div>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="oscillator.detune"
          label="Detune"
          value={detune}
          min={-1200}
          max={1200}
          step={1}
          unit="ct"
          onChange={handleDetuneChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="oscillator.level"
          label="Level"
          value={level}
          min={0}
          max={1}
          step={0.01}
          onChange={handleLevelChange}
          onHover={onParamHover}
        />
      </div>
    </div>
  );
}

const octaveButtonStyle: React.CSSProperties = {
  flex: 1,
  minWidth: 0,
  padding: '4px 0',
  fontSize: '11px',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '3px',
  cursor: 'pointer',
  fontWeight: 600,
};

const panelStyle = {
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '12px',
  minHeight: '160px',
} as const;

const titleStyle = {
  margin: '0 0 8px 0',
  fontSize: '12px',
  fontWeight: 600,
  letterSpacing: '0.1em',
  color: 'var(--color-accent)',
} as const;

const rowLabelStyle = {
  fontSize: '10px',
  color: 'var(--color-text-dim)',
  marginBottom: '4px',
} as const;

const radioLabelStyle = {
  fontSize: '11px',
  color: 'var(--color-text)',
  cursor: 'pointer',
  display: 'flex',
  alignItems: 'center',
} as const;
