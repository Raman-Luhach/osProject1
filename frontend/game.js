/* ===================================================================
   VASUKI Frontend — Renders game using actual C logic via WebAssembly.
   All game logic (math, collision, levels, food, combo) runs in C/WASM.
   This file only handles: rendering (Canvas), input (keyboard), audio.
   =================================================================== */

/* ---- Constants matching game.h ---- */
const GAME_AREA_WIDTH = 60;
const GAME_AREA_HEIGHT = 20;
const MAX_LEVELS = 5;

const DIR_UP = 0;
const DIR_DOWN = 1;
const DIR_LEFT = 2;
const DIR_RIGHT = 3;

const MODE_ENDLESS = 0;
const MODE_LEVELS = 1;

const KEY_UP = 1000;
const KEY_DOWN = 1001;
const KEY_LEFT = 1002;
const KEY_RIGHT = 1003;
const KEY_ESCAPE = 27;

const CELL_SIZE = 16;
const CANVAS_WIDTH = GAME_AREA_WIDTH * CELL_SIZE;
const CANVAS_HEIGHT = GAME_AREA_HEIGHT * CELL_SIZE;

const VERTICAL_SPEED_FACTOR = 1.4;
const BONUS_DURATION_MS = 5000;

/* ---- WASM Module ---- */
let wasm = null;

/* ---- Audio (Web Audio API — replaces audio.c fork+exec) ---- */
const audioCtx = new (window.AudioContext || window.webkitAudioContext)();

function playSound(freq, duration, type) {
    const osc = audioCtx.createOscillator();
    const gain = audioCtx.createGain();
    osc.type = type || 'square';
    osc.frequency.value = freq;
    gain.gain.setValueAtTime(0.08, audioCtx.currentTime);
    gain.gain.exponentialRampToValueAtTime(0.001, audioCtx.currentTime + duration);
    osc.connect(gain);
    gain.connect(audioCtx.destination);
    osc.start();
    osc.stop(audioCtx.currentTime + duration);
}

function soundEat() {
    playSound(600, 0.1, 'square');
    setTimeout(() => playSound(800, 0.1, 'square'), 50);
}

function soundBonus() {
    playSound(500, 0.08, 'sine');
    setTimeout(() => playSound(700, 0.08, 'sine'), 60);
    setTimeout(() => playSound(900, 0.15, 'sine'), 120);
}

function soundDie() {
    playSound(300, 0.15, 'sawtooth');
    setTimeout(() => playSound(200, 0.2, 'sawtooth'), 100);
    setTimeout(() => playSound(100, 0.4, 'sawtooth'), 200);
}

function soundLevel() {
    playSound(400, 0.1, 'sine');
    setTimeout(() => playSound(500, 0.1, 'sine'), 100);
    setTimeout(() => playSound(600, 0.1, 'sine'), 200);
    setTimeout(() => playSound(800, 0.2, 'sine'), 300);
}

/* ---- Particles ---- */
const particlesContainer = document.getElementById('particles-container');

function spawnParticles(canvasX, canvasY, color, count) {
    const canvasRect = canvas.getBoundingClientRect();
    for (let i = 0; i < count; i++) {
        const p = document.createElement('div');
        p.className = 'particle';
        const size = 3 + Math.random() * 5;
        const angle = Math.random() * Math.PI * 2;
        const dist = 20 + Math.random() * 40;
        const dx = Math.cos(angle) * dist;
        const dy = Math.sin(angle) * dist;
        p.style.width = size + 'px';
        p.style.height = size + 'px';
        p.style.background = color;
        p.style.boxShadow = `0 0 6px ${color}`;
        p.style.left = (canvasRect.left + canvasX) + 'px';
        p.style.top = (canvasRect.top + canvasY) + 'px';
        p.style.transform = `translate(${dx}px, ${dy}px)`;
        particlesContainer.appendChild(p);
        setTimeout(() => p.remove(), 800);
    }
}

/* ---- Canvas Renderer (replaces screen.c) ---- */
const canvas = document.getElementById('game-canvas');
const ctx = canvas.getContext('2d');
canvas.width = CANVAS_WIDTH;
canvas.height = CANVAS_HEIGHT;

function getSnakeColor(index, length) {
    const third = Math.floor(length / 3) || 1;
    if (index === 0) return '#00ff88';
    if (index < third) return '#00cc66';
    if (index < third * 2) return '#00aaaa';
    return '#336666';
}

function drawRoundRect(x, y, w, h, r) {
    ctx.beginPath();
    ctx.moveTo(x + r, y);
    ctx.lineTo(x + w - r, y);
    ctx.arcTo(x + w, y, x + w, y + r, r);
    ctx.lineTo(x + w, y + h - r);
    ctx.arcTo(x + w, y + h, x + w - r, y + h, r);
    ctx.lineTo(x + r, y + h);
    ctx.arcTo(x, y + h, x, y + h - r, r);
    ctx.lineTo(x, y + r);
    ctx.arcTo(x, y, x + r, y, r);
    ctx.closePath();
}

function drawGrid() {
    ctx.strokeStyle = 'rgba(255,255,255,0.02)';
    ctx.lineWidth = 0.5;
    for (let x = 0; x <= GAME_AREA_WIDTH; x++) {
        ctx.beginPath();
        ctx.moveTo(x * CELL_SIZE, 0);
        ctx.lineTo(x * CELL_SIZE, CANVAS_HEIGHT);
        ctx.stroke();
    }
    for (let y = 0; y <= GAME_AREA_HEIGHT; y++) {
        ctx.beginPath();
        ctx.moveTo(0, y * CELL_SIZE);
        ctx.lineTo(CANVAS_WIDTH, y * CELL_SIZE);
        ctx.stroke();
    }
}

function drawCell(x, y, color, glow) {
    const px = x * CELL_SIZE;
    const py = y * CELL_SIZE;
    const pad = 1;
    if (glow) { ctx.shadowColor = color; ctx.shadowBlur = 10; }
    ctx.fillStyle = color;
    drawRoundRect(px + pad, py + pad, CELL_SIZE - pad * 2, CELL_SIZE - pad * 2, 3);
    ctx.fill();
    ctx.shadowBlur = 0;
}

function drawObstacles() {
    const lv = wasm._wasm_get_current_level();
    const wallCount = wasm._wasm_get_level_wall_count(lv);
    for (let i = 0; i < wallCount; i++) {
        const wx = wasm._wasm_get_level_wall_x(lv, i);
        const wy = wasm._wasm_get_level_wall_y(lv, i);
        drawCell(wx, wy, '#cc2222', false);
        ctx.fillStyle = '#ff4444';
        ctx.fillRect(wx * CELL_SIZE + 4, wy * CELL_SIZE + 4, CELL_SIZE - 8, CELL_SIZE - 8);
    }
}

function drawSnake() {
    const len = wasm._wasm_get_snake_length();
    const dir = wasm._wasm_get_snake_direction();

    /* Trail */
    const trailCount = wasm._wasm_get_trail_count();
    for (let i = 0; i < trailCount; i++) {
        const tx = wasm._wasm_get_trail_x(i);
        const ty = wasm._wasm_get_trail_y(i);
        ctx.fillStyle = 'rgba(0, 255, 136, 0.06)';
        ctx.fillRect(tx * CELL_SIZE + 3, ty * CELL_SIZE + 3, CELL_SIZE - 6, CELL_SIZE - 6);
    }

    /* Body (reverse so head on top) */
    for (let i = len - 1; i >= 0; i--) {
        const sx = wasm._wasm_get_snake_x(i);
        const sy = wasm._wasm_get_snake_y(i);
        const color = getSnakeColor(i, len);
        drawCell(sx, sy, color, i === 0);

        /* Eyes on head */
        if (i === 0) {
            ctx.fillStyle = '#ffffff';
            let ex = sx * CELL_SIZE + 9, ey = sy * CELL_SIZE + 5;
            switch (dir) {
                case DIR_UP:    ex = sx * CELL_SIZE + 5; ey = sy * CELL_SIZE + 5; break;
                case DIR_DOWN:  ex = sx * CELL_SIZE + 5; ey = sy * CELL_SIZE + 9; break;
                case DIR_LEFT:  ex = sx * CELL_SIZE + 3; ey = sy * CELL_SIZE + 5; break;
                case DIR_RIGHT: ex = sx * CELL_SIZE + 9; ey = sy * CELL_SIZE + 5; break;
            }
            ctx.beginPath(); ctx.arc(ex, ey, 2, 0, Math.PI * 2); ctx.fill();
            ctx.beginPath(); ctx.arc(ex, ey + 4, 2, 0, Math.PI * 2); ctx.fill();
        }
    }
}

function drawFood() {
    if (!wasm._wasm_get_food_active()) return;
    const fx = wasm._wasm_get_food_x();
    const fy = wasm._wasm_get_food_y();
    const tick = wasm._wasm_get_tick_count();
    const blink = (tick % 6) < 3;

    const size = blink ? CELL_SIZE - 4 : CELL_SIZE - 6;
    const offset = (CELL_SIZE - size) / 2;
    const px = fx * CELL_SIZE + offset;
    const py = fy * CELL_SIZE + offset;

    ctx.shadowColor = '#ffcc00';
    ctx.shadowBlur = 12;
    ctx.fillStyle = '#ffcc00';
    ctx.beginPath();
    ctx.arc(px + size / 2, py + size / 2, size / 2, 0, Math.PI * 2);
    ctx.fill();
    ctx.shadowBlur = 0;

    ctx.fillStyle = '#fff';
    ctx.font = `${size - 2}px JetBrains Mono`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText(blink ? '*' : '+', fx * CELL_SIZE + CELL_SIZE / 2, fy * CELL_SIZE + CELL_SIZE / 2);
}

function drawBonusFood() {
    if (!wasm._wasm_get_bonus_active()) return;
    const bx = wasm._wasm_get_bonus_x();
    const by = wasm._wasm_get_bonus_y();
    const spawnTime = wasm._wasm_get_bonus_spawn_time();
    const elapsed = performance.now() - spawnTime;
    const remaining = BONUS_DURATION_MS - elapsed;
    const flash = remaining < 2000 && (Math.floor(elapsed / 200) % 2) === 0;

    if (flash) return;

    const pulse = Math.sin(elapsed * 0.008) * 2;
    const size = CELL_SIZE - 2 + pulse;
    const offset = (CELL_SIZE - size) / 2;
    const px = bx * CELL_SIZE + offset;
    const py = by * CELL_SIZE + offset;

    ctx.shadowColor = '#ff44ff';
    ctx.shadowBlur = 16;
    ctx.fillStyle = '#ff44ff';
    ctx.beginPath();
    ctx.arc(px + size / 2, py + size / 2, size / 2, 0, Math.PI * 2);
    ctx.fill();
    ctx.shadowBlur = 0;

    ctx.fillStyle = '#fff';
    ctx.font = `bold ${CELL_SIZE - 4}px JetBrains Mono`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillText('$', bx * CELL_SIZE + CELL_SIZE / 2, by * CELL_SIZE + CELL_SIZE / 2);
}

function render() {
    ctx.clearRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
    drawGrid();
    drawObstacles();
    drawSnake();
    drawFood();
    drawBonusFood();
}

/* ---- HUD ---- */

function updateHUD() {
    const score = wasm._wasm_get_score();
    const len = wasm._wasm_get_snake_length();
    const hs = wasm._wasm_get_high_score();
    const combo = wasm._wasm_get_combo_multiplier();
    const lv = wasm._wasm_get_current_level();
    const mode = wasm._wasm_get_game_mode();
    const wrap = wasm._wasm_get_level_wrap(lv);

    document.getElementById('hud-score').textContent = `Score: ${score}`;
    document.getElementById('hud-length').textContent = `Len: ${len}`;
    document.getElementById('hud-highscore').textContent = `Best: ${hs}`;

    const comboEl = document.getElementById('hud-combo');
    if (combo > 1) { comboEl.textContent = `x${combo}!`; comboEl.classList.remove('hidden'); }
    else comboEl.classList.add('hidden');

    const goalEl = document.getElementById('hud-goal');
    if (mode === MODE_LEVELS) {
        goalEl.textContent = `Goal: ${wasm._wasm_get_level_target_score(lv)}`;
        goalEl.classList.remove('hidden');
    } else goalEl.classList.add('hidden');

    const wrapper = document.getElementById('canvas-wrapper');
    if (wrap) wrapper.classList.add('wrap-mode');
    else wrapper.classList.remove('wrap-mode');

    const levelEl = document.getElementById('hud-level');
    if (mode === MODE_LEVELS)
        levelEl.textContent = `Level ${lv + 1}${wrap ? ' [WRAP]' : ''}`;
    else {
        let txt = 'Endless';
        if (lv > 0) txt += ` Stage ${lv + 1}`;
        if (wrap) txt += ' [WRAP]';
        levelEl.textContent = txt;
    }
}

/* ---- Screen Management ---- */

function showScreen(id) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));
    document.getElementById(id).classList.add('active');
}

/* ---- High Score (localStorage — replaces fileio.c in browser) ---- */

function loadHighScore() {
    return parseInt(localStorage.getItem('vasuki_highscore') || '0', 10);
}

function saveHighScore(score) {
    localStorage.setItem('vasuki_highscore', score.toString());
}

/* ---- Game Loop ---- */

let gameLoopId = null;
let lastUpdateTime = 0;
let prevScore = 0;
let prevBonusActive = 0;
let prevAlive = 1;

function getFrameDelay() {
    /* C frame_delay is in microseconds, convert to milliseconds */
    let delay = wasm._wasm_get_frame_delay() / 1000;
    const dir = wasm._wasm_get_snake_direction();
    if (dir === DIR_UP || dir === DIR_DOWN)
        delay = Math.round(delay * VERTICAL_SPEED_FACTOR);
    return delay;
}

function gameLoop(timestamp) {
    const running = wasm._wasm_get_running();
    if (!running) {
        onGameEnd();
        return;
    }

    gameLoopId = requestAnimationFrame(gameLoop);

    const elapsed = timestamp - lastUpdateTime;
    const delay = getFrameDelay();
    if (elapsed < delay) return;
    lastUpdateTime = timestamp;

    /* Store state before update for sound triggers */
    prevScore = wasm._wasm_get_score();
    prevBonusActive = wasm._wasm_get_bonus_active();
    prevAlive = wasm._wasm_get_snake_alive();

    /* Run C game logic */
    wasm._wasm_update();

    /* Detect events for audio/particles */
    const newScore = wasm._wasm_get_score();
    const newAlive = wasm._wasm_get_snake_alive();
    const newBonusActive = wasm._wasm_get_bonus_active();

    if (newScore > prevScore && newAlive) {
        const hx = wasm._wasm_get_snake_x(0);
        const hy = wasm._wasm_get_snake_y(0);
        if (prevBonusActive && !newBonusActive) {
            soundBonus();
            spawnParticles(hx * CELL_SIZE + CELL_SIZE / 2, hy * CELL_SIZE + CELL_SIZE / 2, '#ff44ff', 14);
        } else {
            soundEat();
            spawnParticles(hx * CELL_SIZE + CELL_SIZE / 2, hy * CELL_SIZE + CELL_SIZE / 2, '#ffcc00', 8);
        }
    }

    if (prevAlive && !newAlive) {
        soundDie();
    }

    /* Level complete check */
    const result = wasm._wasm_check_level_complete();
    if (result === 1) {
        showLevelComplete();
        return;
    }

    if (wasm._wasm_get_running()) {
        render();
        updateHUD();
    }

    /* Pause overlay */
    const pauseOv = document.getElementById('pause-overlay');
    if (wasm._wasm_get_paused()) pauseOv.classList.remove('hidden');
    else pauseOv.classList.add('hidden');
}

function startGame() {
    lastUpdateTime = performance.now();
    prevScore = 0;
    prevBonusActive = 0;
    prevAlive = 1;
    document.getElementById('pause-overlay').classList.add('hidden');
    document.getElementById('level-complete-overlay').classList.add('hidden');
    render();
    updateHUD();
    gameLoopId = requestAnimationFrame(gameLoop);
}

function stopGame() {
    if (gameLoopId) { cancelAnimationFrame(gameLoopId); gameLoopId = null; }
}

/* ---- Level Complete ---- */

function showLevelComplete() {
    stopGame();
    soundLevel();

    const lv = wasm._wasm_get_current_level();
    const overlay = document.getElementById('level-complete-overlay');
    overlay.querySelector('.level-complete-text').textContent = `LEVEL ${lv + 1} COMPLETE!`;
    overlay.classList.remove('hidden');

    setTimeout(() => {
        overlay.classList.add('hidden');

        const nextLv = lv + 1;
        if (nextLv >= MAX_LEVELS) {
            onGameEnd();
            return;
        }

        /* C function handles reset + level advance */
        wasm._wasm_advance_level();
        startGame();
    }, 2000);
}

/* ---- Death Flash ---- */

function deathFlash(callback) {
    const wrapper = document.getElementById('canvas-wrapper');
    let flashes = 0;
    function flash() {
        if (flashes >= 6) { wrapper.style.borderColor = ''; wrapper.style.boxShadow = ''; callback(); return; }
        if (flashes % 2 === 0) { wrapper.style.borderColor = '#ff3333'; wrapper.style.boxShadow = '0 0 30px rgba(255,51,51,0.4)'; }
        else { wrapper.style.borderColor = ''; wrapper.style.boxShadow = ''; }
        flashes++;
        setTimeout(flash, 80);
    }
    flash();
}

/* ---- Game End ---- */

function onGameEnd() {
    stopGame();
    const score = wasm._wasm_get_score();
    let hs = loadHighScore();

    if (score > hs) { hs = score; saveHighScore(hs); }

    if (!wasm._wasm_get_snake_alive()) {
        deathFlash(() => showGameOver(score, hs));
    } else {
        showGameOver(score, hs);
    }
}

function showGameOver(score, hs) {
    document.getElementById('final-score').textContent = score;
    document.getElementById('final-highscore').textContent = hs;
    const newHsEl = document.getElementById('new-highscore');
    if (score >= hs && score > 0) newHsEl.classList.remove('hidden');
    else newHsEl.classList.add('hidden');
    showScreen('gameover-screen');
}

/* ---- Start New Game (calls C functions) ---- */

function startNewGame(mode, level) {
    wasm._wasm_setup_game(mode, level);
    wasm._wasm_set_high_score(loadHighScore());
    showScreen('game-screen');
    startGame();
}

/* ---- Input (replaces keyboard.c) ---- */

document.addEventListener('keydown', (e) => {
    if (audioCtx.state === 'suspended') audioCtx.resume();
    const key = e.key.toLowerCase();

    if (document.getElementById('welcome-screen').classList.contains('active')) {
        if (key === 'enter') { e.preventDefault(); showScreen('mode-screen'); }
        return;
    }

    if (document.getElementById('mode-screen').classList.contains('active')) {
        if (key === '1') { showScreen('level-screen'); }
        if (key === '2') { startNewGame(MODE_LEVELS, 0); }
        return;
    }

    if (document.getElementById('level-screen').classList.contains('active')) {
        const lvl = parseInt(key) - 1;
        if (lvl >= 0 && lvl < 5) startNewGame(MODE_ENDLESS, lvl);
        return;
    }

    if (document.getElementById('game-screen').classList.contains('active')) {
        e.preventDefault();
        switch (key) {
            case 'w': case 'arrowup':    wasm._wasm_process_input('w'.charCodeAt(0)); break;
            case 's': case 'arrowdown':  wasm._wasm_process_input('s'.charCodeAt(0)); break;
            case 'a': case 'arrowleft':  wasm._wasm_process_input('a'.charCodeAt(0)); break;
            case 'd': case 'arrowright': wasm._wasm_process_input('d'.charCodeAt(0)); break;
            case 'p':                    wasm._wasm_toggle_pause(); break;
            case 'q': case 'escape':     wasm._wasm_process_input('q'.charCodeAt(0)); break;
        }
        return;
    }

    if (document.getElementById('gameover-screen').classList.contains('active')) {
        if (key === 'enter') showScreen('mode-screen');
        if (key === 'q' || key === 'escape') showScreen('welcome-screen');
        return;
    }
});

/* ---- Click Handlers ---- */

document.getElementById('btn-start').addEventListener('click', () => {
    if (audioCtx.state === 'suspended') audioCtx.resume();
    showScreen('mode-screen');
});

document.querySelectorAll('.mode-card').forEach(card => {
    card.addEventListener('click', () => {
        if (card.dataset.mode === 'endless') showScreen('level-screen');
        else startNewGame(MODE_LEVELS, 0);
    });
});

document.querySelectorAll('.level-card').forEach(card => {
    card.addEventListener('click', () => startNewGame(MODE_ENDLESS, parseInt(card.dataset.level)));
});

document.getElementById('btn-replay').addEventListener('click', () => showScreen('mode-screen'));
document.getElementById('btn-quit').addEventListener('click', () => showScreen('welcome-screen'));

/* ---- Initialize WASM Module ---- */

VasukiModule().then(module => {
    wasm = module;

    /* Initialize C subsystems */
    wasm._wasm_init();

    /* Show high score on welcome */
    const hs = loadHighScore();
    const hsEl = document.getElementById('welcome-highscore');
    if (hs > 0) hsEl.textContent = `High Score: ${hs}`;

    showScreen('welcome-screen');

    /* Verify C math is running (you can check console) */
    console.log('C WASM loaded. Verifying math:');
    console.log('  custom_multiply(7, 6) =', wasm._wasm_custom_multiply(7, 6));
    console.log('  custom_divide(100, 7) =', wasm._wasm_custom_divide(100, 7));
    console.log('  custom_modulo(100, 7) =', wasm._wasm_custom_modulo(100, 7));
});
