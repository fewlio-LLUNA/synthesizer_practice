import { useState } from 'react';
import type { SectionProps, UnisonParams, PerformanceParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_UNISON, DEFAULT_PERFORMANCE } from '../../data/defaults';
import { Knob } from '../Knob';

const UNISON_COUNTS = [1, 2, 3, 4, 5, 6, 7, 8];

interface Props extends SectionProps {
  initialUnison?: UnisonParams;
  initialPerformance?: PerformanceParams;
}

export function VoiceSection({ onParamHover, initialUnison, initialPerformance }: Props) {
  const engine = useSynthEngine();
  const u = initialUnison ?? DEFAULT_UNISON;
  const p = initialPerformance ?? DEFAULT_PERFORMANCE;
  const [glide, setGlide] = useState(p.glide);
  const [count, setCount] = useState(u.count);
  const [unisonDetune, setUnisonDetune] = useState(u.detune);

  const handleGlideChange = (value: number) => {
    setGlide(value);
    engine?.setPerformanceParams({ glide: value });
  };

  const handleCountChange = (value: number) => {
    setCount(value);
    engine?.setUnisonParams({ count: value });
  };

  const handleDetuneChange = (value: number) => {
    setUnisonDetune(value);
    engine?.setUnisonParams({ detune: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Voice</h3>
      <div
        style={{ marginBottom: '10px' }}
        onMouseEnter={() => onParamHover('unison.count')}
        onMouseLeave={() => onParamHover(null)}
      >
        <div style={rowLabelStyle}>Unison</div>
        <div style={{ display: 'flex', gap: '2px', flexWrap: 'wrap' }}>
          {UNISON_COUNTS.map((value) => (
            <button
              key={value}
              type="button"
              onClick={() => handleCountChange(value)}
              style={{
                ...countButtonStyle,
                background:
                  count === value ? 'var(--color-accent)' : 'var(--color-knob)',
                color: count === value ? '#1a1a1a' : 'var(--color-text)',
              }}
            >
              {value}
            </button>
          ))}
        </div>
      </div>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="unison.detune"
          label="UnDet"
          value={unisonDetune}
          min={0}
          max={100}
          step={1}
          unit="ct"
          onChange={handleDetuneChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="performance.glide"
          label="Glide"
          value={glide}
          min={0}
          max={2}
          step={0.01}
          unit="s"
          onChange={handleGlideChange}
          onHover={onParamHover}
        />
      </div>
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

const countButtonStyle: React.CSSProperties = {
  width: '22px',
  padding: '4px 0',
  fontSize: '10px',
  border: '1px solid var(--color-panel-border)',
  borderRadius: '3px',
  cursor: 'pointer',
  fontWeight: 600,
};
