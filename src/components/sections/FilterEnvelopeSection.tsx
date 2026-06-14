import { useState } from 'react';
import type { SectionProps, FilterEnvelopeParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_FILTER_ENVELOPE } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialParams?: FilterEnvelopeParams;
}

export function FilterEnvelopeSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_FILTER_ENVELOPE;
  const [attack, setAttack] = useState(init.attack);
  const [decay, setDecay] = useState(init.decay);
  const [sustain, setSustain] = useState(init.sustain);
  const [release, setRelease] = useState(init.release);
  const [amount, setAmount] = useState(init.amount);

  const handleAttackChange = (value: number) => {
    setAttack(value);
    engine?.setFilterEnvelopeParams({ attack: value });
  };

  const handleDecayChange = (value: number) => {
    setDecay(value);
    engine?.setFilterEnvelopeParams({ decay: value });
  };

  const handleSustainChange = (value: number) => {
    setSustain(value);
    engine?.setFilterEnvelopeParams({ sustain: value });
  };

  const handleReleaseChange = (value: number) => {
    setRelease(value);
    engine?.setFilterEnvelopeParams({ release: value });
  };

  const handleAmountChange = (value: number) => {
    setAmount(value);
    engine?.setFilterEnvelopeParams({ amount: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Filter Env</h3>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="filter.envelope.amount"
          label="Amt"
          value={amount}
          min={-1}
          max={1}
          step={0.01}
          onChange={handleAmountChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="filter.envelope.attack"
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
          paramId="filter.envelope.decay"
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
          paramId="filter.envelope.sustain"
          label="S"
          value={sustain}
          min={0}
          max={1}
          step={0.01}
          onChange={handleSustainChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="filter.envelope.release"
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
