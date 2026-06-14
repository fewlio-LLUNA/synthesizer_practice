import { useState } from 'react';
import type { SectionProps, DelayParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_DELAY } from '../../data/defaults';
import { Knob } from '../Knob';

interface Props extends SectionProps {
  initialParams?: DelayParams;
}

export function DelaySection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_DELAY;
  const [time, setTime] = useState(init.time);
  const [feedback, setFeedback] = useState(init.feedback);
  const [mix, setMix] = useState(init.mix);

  const handleTimeChange = (value: number) => {
    setTime(value);
    engine?.setDelayParams({ time: value });
  };

  const handleFeedbackChange = (value: number) => {
    setFeedback(value);
    engine?.setDelayParams({ feedback: value });
  };

  const handleMixChange = (value: number) => {
    setMix(value);
    engine?.setDelayParams({ mix: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Delay</h3>
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="delay.time"
          label="Time"
          value={time}
          min={0.01}
          max={2}
          step={0.01}
          unit="s"
          onChange={handleTimeChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="delay.feedback"
          label="Fbk"
          value={feedback}
          min={0}
          max={0.95}
          step={0.01}
          onChange={handleFeedbackChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="delay.mix"
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
