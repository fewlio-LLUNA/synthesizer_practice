import { useState } from 'react';
import type { SectionProps, ReverbParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_REVERB } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialParams?: ReverbParams;
}

export function ReverbSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_REVERB;
  const [decay, setDecay] = useState(init.decay);
  const [mix, setMix] = useState(init.mix);

  const handleDecayChange = (value: number) => {
    setDecay(value);
    engine?.setReverbParams({ decay: value });
  };

  const handleMixChange = (value: number) => {
    setMix(value);
    engine?.setReverbParams({ mix: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Reverb</h3>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="reverb.decay"
          label="Decay"
          value={decay}
          min={0.1}
          max={10}
          step={0.1}
          unit="s"
          onChange={handleDecayChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="reverb.mix"
          label="Mix"
          value={mix}
          min={0}
          max={1}
          step={0.01}
          onChange={handleMixChange}
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
