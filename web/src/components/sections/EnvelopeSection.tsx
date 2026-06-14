import { useState } from 'react';
import type { SectionProps, EnvelopeParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_ENVELOPE } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialParams?: EnvelopeParams;
}

export function EnvelopeSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_ENVELOPE;
  const [attack, setAttack] = useState(init.attack);
  const [decay, setDecay] = useState(init.decay);
  const [sustain, setSustain] = useState(init.sustain);
  const [release, setRelease] = useState(init.release);

  const handleAttackChange = (value: number) => {
    setAttack(value);
    engine?.setEnvelopeParams({ attack: value });
  };

  const handleDecayChange = (value: number) => {
    setDecay(value);
    engine?.setEnvelopeParams({ decay: value });
  };

  const handleSustainChange = (value: number) => {
    setSustain(value);
    engine?.setEnvelopeParams({ sustain: value });
  };

  const handleReleaseChange = (value: number) => {
    setRelease(value);
    engine?.setEnvelopeParams({ release: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Amp Env</h3>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="envelope.attack"
          label="A"
          value={attack}
          min={0.001}
          max={5}
          step={0.001}
          unit="s"
          onChange={handleAttackChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="envelope.decay"
          label="D"
          value={decay}
          min={0.001}
          max={5}
          step={0.001}
          unit="s"
          onChange={handleDecayChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="envelope.sustain"
          label="S"
          value={sustain}
          min={0}
          max={1}
          step={0.01}
          onChange={handleSustainChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="envelope.release"
          label="R"
          value={release}
          min={0.001}
          max={5}
          step={0.001}
          unit="s"
          onChange={handleReleaseChange}
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
