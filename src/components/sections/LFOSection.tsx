import { useState } from 'react';
import type { SectionProps, LFOTarget, WaveformType } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_LFO } from '../../data/defaults';
import { Knob } from '../Knob';

const WAVEFORMS: { value: WaveformType; label: string }[] = [
  { value: 'sine', label: 'Sin' },
  { value: 'sawtooth', label: 'Saw' },
  { value: 'square', label: 'Sqr' },
  { value: 'triangle', label: 'Tri' },
];

const LFO_TARGETS: { value: LFOTarget; label: string }[] = [
  { value: 'pitch', label: 'Pitch' },
  { value: 'filter', label: 'Filter' },
  { value: 'amp', label: 'Amp' },
];

export function LFOSection({ onParamHover }: SectionProps) {
  const engine = useSynthEngine();
  const [rate, setRate] = useState(DEFAULT_LFO.rate);
  const [depth, setDepth] = useState(DEFAULT_LFO.depth);
  const [target, setTarget] = useState<LFOTarget>(DEFAULT_LFO.target);
  const [waveform, setWaveform] = useState<WaveformType>(DEFAULT_LFO.waveform);

  const handleRateChange = (value: number) => {
    setRate(value);
    engine?.setLFOParams({ rate: value });
  };

  const handleDepthChange = (value: number) => {
    setDepth(value);
    engine?.setLFOParams({ depth: value });
  };

  const handleTargetChange = (value: LFOTarget) => {
    setTarget(value);
    engine?.setLFOParams({ target: value });
  };

  const handleWaveformChange = (value: WaveformType) => {
    setWaveform(value);
    engine?.setLFOParams({ waveform: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>LFO</h3>
      <div style={{ marginBottom: '6px' }}>
        <div style={rowLabelStyle}>Waveform</div>
        <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
          {WAVEFORMS.map(({ value, label }) => (
            <label key={value} style={radioLabelStyle}>
              <input
                type="radio"
                name="lfo-waveform"
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
      <div style={{ marginBottom: '8px' }}>
        <div style={rowLabelStyle}>Target</div>
        <div style={{ display: 'flex', gap: '8px' }}>
          {LFO_TARGETS.map(({ value, label }) => (
            <label key={value} style={radioLabelStyle}>
              <input
                type="radio"
                name="lfo-target"
                value={value}
                checked={target === value}
                onChange={() => handleTargetChange(value)}
                style={{ marginRight: '3px' }}
              />
              {label}
            </label>
          ))}
        </div>
      </div>
      <div style={{ display: 'flex', gap: '8px' }}>
        <Knob
          paramId="lfo.rate"
          label="Rate"
          value={rate}
          min={0.1}
          max={20}
          step={0.1}
          unit="Hz"
          onChange={handleRateChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="lfo.depth"
          label="Depth"
          value={depth}
          min={0}
          max={1}
          step={0.01}
          onChange={handleDepthChange}
          onHover={onParamHover}
        />
      </div>
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
  marginBottom: '3px',
} as const;

const radioLabelStyle = {
  fontSize: '11px',
  color: 'var(--color-text)',
  cursor: 'pointer',
  display: 'flex',
  alignItems: 'center',
} as const;
