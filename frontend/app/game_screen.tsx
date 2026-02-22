'use client'
import { useEffect, useRef } from "react";

let inputSeq: number = 0;

function sendInput() {

}

const inputs = ['w', 'a', 's', 'd'];

type ClientInputMessage = {
        playerId: number;
        keyDown: boolean;
        key: string;
}

function pack_message(msg: ClientInputMessage): ArrayBuffer {
        const out = new ArrayBuffer(3);
        const dv = new DataView(out);
        let offset = 0;
        dv.setUint8(offset, msg.playerId);
        offset += 1;
        dv.setUint8(offset, msg.keyDown ? 1 : 0);
        offset += 1;
        dv.setUint8(offset, msg.key.charCodeAt(0));
        return out;
}

type PlayerState = {
        x: number;
        y: number;
}

type GameState = {
        playerStates: PlayerState[];
}

function parse_game_state(data: ArrayBuffer): GameState {
        const gameState: GameState = { playerStates: [] };
        const dv = new DataView(data);
        let offset = 0;
        const numPlayers = dv.getInt8(offset);
        offset += 1;
        for (let _ = 0; _ < numPlayers; _++) {
                const x = dv.getFloat32(offset, true);
                offset += 4;
                const y = dv.getFloat32(offset, true);
                offset += 4;
                gameState.playerStates.push({ x, y });
        }
        return gameState;
}

export default function GameScreen() {
        const keyRef = useRef<Record<string, boolean>>({});
        const wsRef = useRef<WebSocket | null>(null);
        useEffect(() => {
                const handleKeyDown = (e: KeyboardEvent) => {
                        if (inputs.includes(e.key)) {
                                e.preventDefault();
                        } else {
                                return;
                        }
                        if (keyRef.current[e.key]) return;
                        keyRef.current[e.key] = true;
                        const msg: ClientInputMessage = { playerId: 0, key: e.key, keyDown: true };
                        console.log(`msg: playerId=${msg.playerId}, keyDown=${msg.keyDown}, key=${msg.key}`);
                        wsRef.current?.send(pack_message(msg));
                }
                const handleKeyUp = (e: KeyboardEvent) => {
                        if (inputs.includes(e.key)) {
                                e.preventDefault();
                        } else {
                                return;
                        }
                        if (!keyRef.current[e.key]) return;
                        keyRef.current[e.key] = false;
                        const msg: ClientInputMessage = { playerId: 0, key: e.key, keyDown: false };
                        console.log(`msg: playerId=${msg.playerId}, keyDown=${msg.keyDown}, key=${msg.key}`);
                        wsRef.current?.send(pack_message(msg));
                }
                window.addEventListener('keydown', handleKeyDown);
                window.addEventListener('keyup', handleKeyUp);
                return () => {
                        window.removeEventListener('keydown', handleKeyDown);
                        window.removeEventListener('keyup', handleKeyUp);
                }
        }, []);
        useEffect(() => {
                wsRef.current = new WebSocket("ws://localhost:8080");
                wsRef.current.binaryType = 'arraybuffer';
                wsRef.current.addEventListener('open', (event) => {
                        console.log('web socket opened');
                });
                wsRef.current.addEventListener('message', (event) => {
                        const buf = event.data;
                        const dv = new DataView(buf);
                        console.log("bytes received:", buf.byteLength);
                        for (let i = 0; i < buf.byteLength; i++) {
                                console.log(`byte[${i}] = ${dv.getUint8(i)}`);
                        }
                        const gameState = parse_game_state(event.data);
                        console.log(gameState);
                });
                return () => {
                        if (wsRef.current)
                                wsRef.current.close();
                };
        }, []);
        return (
                <div>
                        <script src="game_canvas_script.ts" />
                        <canvas id="gameCanvas" width="600" height="400" ></canvas>
                </div>
        );
}
