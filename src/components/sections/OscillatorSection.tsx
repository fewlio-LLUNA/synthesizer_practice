import { useState } from 'react';
import type { SectionProps, WaveformType } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_OSCILLATOR } from '../../data/defaults';
import { Knob } from '../Knob';

const WAVEFORMS: { value: WaveformType; label: string }[] = [
  { value: 'sine', label: 'Sin' },
  { value: 'sawtooth', label: 'Saw' },
  { value: 'square', label: 'Sqr' },
  { value: 'triangle', label: 'Tri' },
];

export function OscillatorSection({ onParamHover }: SectionProps) {
  const engine = useSynthEngine();
  const [waveform, setWaveform] = useState<WaveformType>(DEFAULT_OSCILLATOR.waveform);
  const [detune, setDetune] = useState(DEFAULT_OSCILLATOR.detune);

  const handleWaveformChange = (value: WaveformType) => {
    setWaveform(value);
    engine?.setOscillatorParams({ waveform: value });
  };

  const handleDetuneChange = (value: number) => {
    setDetune(value);
    engine?.setOscillatorParams({ detune: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Oscillator</h3>
      <div style={{ marginBottom: '10px' }}>
        <div style={rowLabelStyle}>Waveform</div>
        <div style={{ display: 'flex', gap: '6px', flexWrap: 'wrap' }}>
          {WAVEFORMS.map(({ value, label }) => (
            <label key={value} style={radioLabelStyle}>
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
    </div>
  );
}

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
