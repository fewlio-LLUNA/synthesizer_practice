import { useState } from 'react';
import type { SectionProps, FilterType } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_FILTER } from '../../data/defaults';
import { Knob } from '../Knob';

const FILTER_TYPES: { value: FilterType; label: string }[] = [
  { value: 'lowpass', label: 'LP' },
  { value: 'highpass', label: 'HP' },
  { value: 'bandpass', label: 'BP' },
];

export function FilterSection({ onParamHover }: SectionProps) {
  const engine = useSynthEngine();
  const [filterType, setFilterType] = useState<FilterType>(DEFAULT_FILTER.type);
  const [cutoff, setCutoff] = useState(DEFAULT_FILTER.cutoff);
  const [resonance, setResonance] = useState(DEFAULT_FILTER.resonance);

  const handleTypeChange = (value: FilterType) => {
    setFilterType(value);
    engine?.setFilterParams({ type: value });
  };

  const handleCutoffChange = (value: number) => {
    setCutoff(value);
    engine?.setFilterParams({ cutoff: value });
  };

  const handleResonanceChange = (value: number) => {
    setResonance(value);
    engine?.setFilterParams({ resonance: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Filter</h3>
      <div style={{ marginBottom: '10px' }}>
        <div style={rowLabelStyle}>Type</div>
        <div style={{ display: 'flex', gap: '8px' }}>
          {FILTER_TYPES.map(({ value, label }) => (
            <label key={value} style={radioLabelStyle}>
              <input
                type="radio"
                name="filter-type"
                value={value}
                checked={filterType === value}
                onChange={() => handleTypeChange(value)}
                style={{ marginRight: '3px' }}
              />
              {label}
            </label>
          ))}
        </div>
      </div>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="filter.cutoff"
          label="Cutoff"
          value={cutoff}
          min={20}
          max={20000}
          step={1}
          unit="Hz"
          logarithmic
          onChange={handleCutoffChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="filter.resonance"
          label="Resonance"
          value={resonance}
          min={0.1}
          max={30}
          step={0.1}
          onChange={handleResonanceChange}
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
  marginBottom: '4px',
} as const;

const radioLabelStyle = {
  fontSize: '11px',
  color: 'var(--color-text)',
  cursor: 'pointer',
  display: 'flex',
  alignItems: 'center',
} as const;
