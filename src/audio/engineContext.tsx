// SynthEngine を React ツリー全体に提供する Context。
// 各 Section コンポーネントは useSynthEngine() で取得して使う。
//
// このファイルは共有資産。Session A と Session D の境界に位置する。
// Provider のセットアップは Session A の責務、利用は Session D。

import { createContext, useContext, type ReactNode } from 'react';
import type { ISynthEngine } from '../types';

const SynthEngineContext = createContext<ISynthEngine | null>(null);

export function SynthEngineProvider({
  engine,
  children,
}: {
  engine: ISynthEngine;
  children: ReactNode;
}) {
  return (
    <SynthEngineContext.Provider value={engine}>
      {children}
    </SynthEngineContext.Provider>
  );
}

/**
 * SynthEngine を取得するフック。
 * Provider 外で呼ぶと null を返すので、スタブ動作時にも壊れない。
 */
export function useSynthEngine(): ISynthEngine | null {
  return useContext(SynthEngineContext);
}
