import { useEffect, useMemo, useRef, useState } from 'react';

type Cell = [number, number];

type SolveResponse = {
  success: boolean;
  path: Cell[];
  message: string;
};

const INITIAL_ROWS = 8;
const INITIAL_COLS = 7;
const MAX_SIZE = 30;

const INITIAL_BLOCKED: Cell[] = [
  [0, 4], [1, 6], [2, 3], [2, 4], [2, 5], [2, 6],
  [3, 0], [3, 1], [3, 4], [3, 5], [3, 6],
  [4, 0], [4, 1], [4, 2],
  [5, 0], [5, 1], [5, 2], [5, 3], [5, 4], [5, 5], [5, 6],
  [6, 0], [6, 1], [6, 2], [6, 3], [6, 4], [6, 5], [6, 6],
  [7, 0], [7, 1], [7, 2], [7, 3], [7, 4], [7, 5], [7, 6],
];

const key = ([r, c]: Cell) => `${r},${c}`;

function getInitialBlocked() {
  return new Set(INITIAL_BLOCKED.map(key));
}

export default function App() {
  const [rows, setRows] = useState(INITIAL_ROWS);
  const [cols, setCols] = useState(INITIAL_COLS);

  // Input nháp: cho phép người dùng nhập kích thước trước khi áp dụng.
  const [rowsInput, setRowsInput] = useState(String(INITIAL_ROWS));
  const [colsInput, setColsInput] = useState(String(INITIAL_COLS));
  const boardWrapperRef = useRef<HTMLDivElement>(null);
  const [linePoints, setLinePoints] = useState('');
  const [start, setStart] = useState<Cell>([4, 6]);
  const [blocked, setBlocked] = useState<Set<string>>(getInitialBlocked);
  const [path, setPath] = useState<Cell[]>([]);
  const [status, setStatus] = useState(
    'Chọn ô để bật/tắt trạng thái bị chặn, sau đó nhấn Solve.'
  );
  const [loading, setLoading] = useState(false);
  const [step, setStep] = useState(0);
  const [svgSize, setSvgSize] = useState({
    width: 0,
    height: 0,
  });

  useEffect(() => {
    const wrapper = boardWrapperRef.current;

    if (!wrapper) return;

    const updateLine = () => {
      const board = wrapper.querySelector('.board');
      if (!board) return;

      const boardRect = board.getBoundingClientRect();
      setSvgSize({ width: boardRect.width, height: boardRect.height });

      const tiles = wrapper.querySelectorAll<HTMLButtonElement>('.tile');

      // Chỉ vẽ tới bước hiện tại trên thanh stepper.
      // const visiblePath = path.slice(0, step + 1);
      const visiblePath = path;

      const points = visiblePath
        .map(([r, c]) => {
          const index = r * cols + c;
          const tile = tiles[index];

          if (!tile) return null;

          const rect = tile.getBoundingClientRect();

          // SVG nằm phủ trên board nên tọa độ cũng phải tính từ board.
          const x = rect.left - boardRect.left + rect.width / 2;
          const y = rect.top - boardRect.top + rect.height / 2;

          return `${x},${y}`;
        })
        .filter((point): point is string => point !== null);
      setLinePoints(points.join(' '));

    };

    updateLine();
    const observer = new ResizeObserver(updateLine);
    observer.observe(wrapper);
    const board = wrapper.querySelector('.board');
    if (board) observer.observe(board);
    return () => observer.disconnect();
  }, [path, step, rows, cols]);

  const pathSet = useMemo(() => new Set(path.map(key)), [path]);

  const pathIndex = useMemo(
    () => new Map(path.map((p, i) => [key(p), i])),
    [path]
  );

  function clearSolution() {
    setPath([]);
    setStep(0);
  }

  function toggleCell(r: number, c: number) {
    const k = `${r},${c}`;

    if (r === start[0] && c === start[1]) {
      setStatus('Ô bắt đầu không thể bị chặn.');
      return;
    }

    setBlocked(prev => {
      const next = new Set(prev);

      if (next.has(k)) {
        next.delete(k);
      } else {
        next.add(k);
      }

      return next;
    });

    clearSolution();
    setStatus('Bàn cờ đã thay đổi. Hãy chạy Solve lại.');
  }

  function chooseStart(r: number, c: number) {
    if (blocked.has(`${r},${c}`)) {
      setStatus('Hãy bỏ chặn ô này trước khi chọn làm điểm bắt đầu.');
      return;
    }

    setStart([r, c]);
    clearSolution();
    setStatus(`Điểm bắt đầu mới: (${r}, ${c}).`);
  }

  function applySize() {
    const newRows = Number(rowsInput);
    const newCols = Number(colsInput);

    if (
      !Number.isInteger(newRows) ||
      !Number.isInteger(newCols) ||
      newRows < 1 ||
      newRows > MAX_SIZE ||
      newCols < 1 ||
      newCols > MAX_SIZE
    ) {
      setStatus(`Số hàng và số cột phải nằm trong khoảng 1–${MAX_SIZE}.`);
      return;
    }

    setRows(newRows);
    setCols(newCols);

    // Đưa điểm bắt đầu về góc trên bên trái.
    setStart([0, 0]);

    // Bàn cờ mới bắt đầu với tất cả ô đều mở.
    setBlocked(new Set());

    clearSolution();
    setStatus(`Đã tạo bàn cờ ${newRows} × ${newCols}.`);
  }

  function resetBoard() {
    setRows(INITIAL_ROWS);
    setCols(INITIAL_COLS);

    setRowsInput(String(INITIAL_ROWS));
    setColsInput(String(INITIAL_COLS));

    setStart([4, 6]);
    setBlocked(getInitialBlocked());

    clearSolution();
    setStatus('Bàn cờ demo đã được đặt lại.');
  }

  async function solve() {
    if (loading) return;

    setLoading(true);
    clearSolution();
    setStatus('Đang gửi bàn cờ đến C++ solver…');

    try {
      const response = await fetch('/api/solve', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          rows,
          cols,
          start,
          blocked: [...blocked].map(s => s.split(',').map(Number)),
        }),
      });

      if (!response.ok) {
        throw new Error(await response.text());
      }

      const result: SolveResponse = await response.json();
      const resultPath = result.path ?? [];

      setPath(resultPath);
      setStep(0);

      setStatus(
        result.success
          ? `Tìm thấy đường đi gồm ${resultPath.length} ô!`
          : 'Không tìm thấy đường đi cho bàn cờ này.'
      );
    } catch (err) {
      setStatus(
        `Không kết nối được backend: ${err instanceof Error ? err.message : String(err)
        }. Kiểm tra C++ server và API /api/solve.`
      );
    } finally {
      setLoading(false);
    }
  }

  return (
    <main className="app-shell">
      <header className="topbar">
        <div className="brand-mark">↗</div>

        <div>
          <p className="eyebrow">C++ · CROW · REACT</p>
          <h1>
            One Line <span>Solver</span>
          </h1>
        </div>

        <div className="live">
          <i /> LOCAL SOLVER
        </div>
      </header>

      <section className="intro">
        <div>
          <p className="eyebrow">BACKTRACKING LAB</p>
          <h2>Đưa thuật toán ra khỏi terminal.</h2>
          <p className="muted">
            Chọn bàn cờ, gọi DFS từ C++ và xem đường đi được render ngay tại đây.
          </p>
        </div>

        <button
          className="solve-btn"
          onClick={solve}
          disabled={loading}
        >
          {loading ? 'Đang giải…' : '▶ Solve board'}
        </button>
      </section>

      <section className="workspace">
        <div className="board-panel">
          <div className="panel-heading">
            <div>
              <h3>Bàn cờ</h3>
              <p>
                {rows} × {cols} · Tọa độ bắt đầu từ 0
              </p>
            </div>

            <span className="pill">
              {path.length ? `${path.length} ô trong nghiệm` : 'Chưa có nghiệm'}
            </span>
          </div>


          <div className="board-settings">
            <label htmlFor="board-rows">
              <span className="setting-label">
                <span className="setting-icon">↕</span>
                SỐ HÀNG
              </span>
              <input
                id="board-rows"
                type="number"
                min={1}
                max={30}
                value={rowsInput}
                onChange={e => setRowsInput(e.target.value)}
                onKeyDown={e => {
                  if (e.key === 'Enter') applySize();
                }}
                aria-label="Số hàng bàn cờ"
              />
            </label>

            <label htmlFor="board-cols">
              <span className="setting-label">
                <span className="setting-icon">↔</span>
                SỐ CỘT
              </span>
              <input
                id="board-cols"
                type="number"
                min={1}
                max={30}
                value={colsInput}
                onChange={e => setColsInput(e.target.value)}
                onKeyDown={e => {
                  if (e.key === 'Enter') applySize();
                }}
                aria-label="Số cột bàn cờ"
              />
            </label>

            <button
              type="button"
              className="secondary-btn apply-size-btn"
              onClick={applySize}
              disabled={loading}
            >
              <span>↻</span> Áp dụng
            </button>

            <div className="board-size-info">
              <span className="size-number">{rows * cols}</span>
              <span className="size-caption">Ô trên bàn cờ</span>
            </div>
          </div>

          <div className="board-wrapper" ref={boardWrapperRef}>
            <div
              className="board"
              style={{
                gridTemplateColumns: `repeat(${cols}, minmax(0, 1fr))`,
              }}
            >
              {Array.from({ length: rows * cols }, (_, index) => {
                const r = Math.floor(index / cols);
                const c = index % cols;
                const k = `${r},${c}`;

                const isStart = r === start[0] && c === start[1];
                const isBlocked = blocked.has(k);
                const isPath = pathSet.has(k);

                return (
                  <button
                    key={k}
                    type="button"
                    title={`(${r}, ${c})`}
                    className={`tile ${isBlocked ? 'blocked' : 'open'} ${isPath ? 'on-path' : ''
                      } ${isStart ? 'start' : ''}`}
                    onClick={e =>
                      e.shiftKey ? chooseStart(r, c) : toggleCell(r, c)
                    }
                  >
                    {isStart && <span className="start-dot" />}
                  </button>
                );
              })}
              <svg
                className="path-overlay"
                viewBox={`0 0 ${svgSize.width || 1} ${svgSize.height || 1}`}
                preserveAspectRatio="none"
                style={{
                  position: 'absolute',
                  top: 0,
                  left: 0,
                  width: '100%',
                  height: '100%',
                  zIndex: 2,
                  pointerEvents: 'none',
                  overflow: 'hidden',
                }}
              >
                <polyline
                  points={linePoints}
                  fill="none"
                  stroke="#ffcb5c"
                  strokeWidth="5"
                  strokeLinecap="round"
                  strokeLinejoin="round"
                />
              </svg>
            </div>
          </div>
          <p className="hint">
            Click ô để chặn/bỏ chặn · Shift + click để chọn điểm bắt đầu.
          </p>
        </div>

        <aside className="side-panel">
          <div className="panel-heading">
            <div>
              <h3>Kết quả solver</h3>
              <p>POST /api/solve</p>
            </div>

            <span className={`status-dot ${loading ? 'busy' : ''}`} />
          </div>

          <div className="status-box">{status}</div>

          <div className="stats">
            <div>
              <span>Số ô trên đường</span>
              <strong>{path.length}</strong>
            </div>

            <div>
              <span>Số bước</span>
              <strong>{Math.max(0, path.length - 1)}</strong>
            </div>

            <div>
              <span>Điểm bắt đầu</span>
              <strong>
                ({start[0]}, {start[1]})
              </strong>
            </div>
          </div>

          {path.length > 0 && (
            <div className="stepper">
              <div className="stepper-top">
                <span>Đường đi</span>
                <span>
                  {Math.min(step + 1, path.length)} / {path.length}
                </span>
              </div>

              <input
                type="range"
                min={0}
                max={path.length - 1}
                value={Math.min(step, path.length - 1)}
                onChange={e => setStep(Number(e.target.value))}
              />

              <p>
                Ô hiện tại:{' '}
                <b>
                  ({path[Math.min(step, path.length - 1)][0]},{' '}
                  {path[Math.min(step, path.length - 1)][1]})
                </b>
              </p>

              <button
                className="secondary-btn"
                onClick={() => setStep(s => (s + 1) % path.length)}
              >
                Bước tiếp theo →
              </button>
            </div>
          )}

          <div className="legend">
            <span>
              <i className="legend-open" /> Ô có thể đi
            </span>
            <span>
              <i className="legend-blocked" /> Ô bị chặn
            </span>
            <span>
              <i className="legend-path" /> Đường đi
            </span>
          </div>

          <button className="reset-btn" onClick={resetBoard}>
            Reset demo board
          </button>
        </aside>
      </section>

      <footer>
        Built with C++ DFS · Crow REST API · React + TypeScript
      </footer>
    </main>
  );
}
