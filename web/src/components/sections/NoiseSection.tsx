import { useState } from 'react';
import type { SectionProps, NoiseType, NoiseParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_NOISE } from '../../data/defaults';
import { Knob } from '../Knob';

const NOISE_TYPES: { value: NoiseType; label: string; paramId: string }[] = [
  { value: 'white', label: 'White', paramId: 'noise.type.white' },
  { value: 'pink', label: 'Pink', paramId: 'noise.type.pink' },
];

interface Props extends SectionProps {
  initialParams?: NoiseParams;
}

export function NoiseSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_NOISE;
  const [type, setType] = useState<NoiseType>(init.type);
  const [level, setLevel] = useState(init.level);

  const handleTypeChange = (value: NoiseType) => {
    setType(value);
    engine?.setNoiseParams({ type: value });
  };

  const handleLevelChange = (value: number) => {
    setLevel(value);
    engine?.setNoiseParams({ level: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Noise</h3>
      <div
        style={{ marginBottom: '10px' }}
        onMouseEnter={() => onParamHover('noise.type')}
        onMouseLeave={() => onParamHover(null)}
      >
        <div style={rowLabelStyle}>Type</div>
        <div style={{ display: 'flex', gap: '8px' }}>
          {NOISE_TYPES.map(({ value, label, paramId }) => (
            <label
              key={value}
              style={radioLabelStyle}
              onMouseEnter={(e) => {
                e.stopPropagation();
                onParamHover(paramId);
              }}
              onMouseLeave={(e) => {
                e.stopPropagation();
                onParamHover('noise.type');
              }}
            >
              <input
                type="radio"
                name="noise-type"
                value={value}
                checked={type === value}
                onChange={() => handleTypeChange(value)}
                style={{ marginRight: '3px' }}
              />
              {label}
            </label>
          ))}
        </div>
      </div>
      <Knob
        paramId="noise.level"
        label="Level"
        value={level}
        min={0}
        max={1}
        step={0.01}
        onChange={handleLevelChange}
        onHover={onParamHover}
      />
    </div>
  );
}

const panelStyle: React.CSSProperties = {
  background: 'var(--color-panel)',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '4px',
  padding: '12px',
  minHeight: '160px',
};

const titleStyle: React.CSSProperties = {
  margin: '0 0 8px 0',
  fontSize: '12px',
  fontWeight: 600,
  letterSpacing: '0.1em',
  color: 'var(--color-accent)',
};

const rowLabelStyle: React.CSSProperties = {
  fontSize: '10px',
  color: 'var(--color-text-dim)',
  marginBottom: '4px',
};

const radioLabelStyle: React.CSSProperties = {
  fontSize: '11px',
  color: 'var(--color-text)',
  cursor: 'pointer',
  display: 'flex',
  alignItems: 'center',
};
