// TODO: Session E 担当 — パラメータ解説文
//
// 各つまみの解説を集約。InfoPanel.tsx が paramId で検索する。
// 解説は「数値の意味」と「人間の感覚での意味」を併記すること。

import type { ParameterDescription } from '../types';

export const descriptions: ParameterDescription[] = [
  // サンプルとして1件だけ用意。Session E がすべてのパラメータについて執筆する。
  {
    id: 'filter.cutoff',
    name: 'カットオフ周波数',
    summary: 'フィルターが効き始める基準周波数。',
    detail:
      'ローパスフィルターでは、この周波数より上の成分を減衰させる。\n下げると音がこもり、上げると元の波形に近い明るい音になる。',
    unit: 'Hz',
  },
];
