import { useState } from 'react';
import type { SectionProps, DriveType, DriveParams } from '../../types';
import { useSynthEngine } from '../../audio/engineContext';
import { DEFAULT_DRIVE } from '../../data/defaults';
import { Knob } from '../Knob';

const DRIVE_TYPES: { value: DriveType; label: string; paramId: string }[] = [
  { value: 'soft', label: 'Soft', paramId: 'drive.type.soft' },
  { value: 'hard', label: 'Hard', paramId: 'drive.type.hard' },
];

interface Props extends SectionProps {
  initialParams?: DriveParams;
}

export function DriveSection({ onParamHover, initialParams }: Props) {
  const engine = useSynthEngine();
  const init = initialParams ?? DEFAULT_DRIVE;
  const [type, setType] = useState<DriveType>(init.type);
  const [drive, setDrive] = useState(init.drive);
  const [mix, setMix] = useState(init.mix);

  const handleTypeChange = (value: DriveType) => {
    setType(value);
    engine?.setDriveParams({ type: value });
  };

  const handleDriveChange = (value: number) => {
    setDrive(value);
    engine?.setDriveParams({ drive: value });
  };

  const handleMixChange = (value: number) => {
    setMix(value);
    engine?.setDriveParams({ mix: value });
  };

  return (
    <div style={panelStyle}>
      <h3 style={titleStyle}>Drive</h3>
      <div
        style={{ marginBottom: '10px' }}
        onMouseEnter={() => onParamHover('drive.type')}
        onMouseLeave={() => onParamHover(null)}
      >
        <div style={rowLabelStyle}>Type</div>
        <div style={{ display: 'flex', gap: '8px' }}>
          {DRIVE_TYPES.map(({ value, label, paramId }) => (
            <label
              key={value}
              style={radioLabelStyle}
              onMouseEnter={(e) => {
                e.stopPropagation();
                onParamHover(paramId);
              }}
              onMouseLeave={(e) => {
                e.stopPropagation();
                onParamHover('drive.type');
              }}
            >
              <input
                type="radio"
                name="drive-type"
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
      <div style={{ display: 'flex', gap: '4px', flexWrap: 'wrap' }}>
        <Knob
          paramId="drive.drive"
          label="Drive"
          value={drive}
          min={0}
          max={1}
          step={0.01}
          onChange={handleDriveChange}
          onHover={onParamHover}
        />
        <Knob
          paramId="drive.mix"
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
