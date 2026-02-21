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
                        console.log(`KeyUp: ${e.key}`);
                }
                window.addEventListener('keydown', handleKeyDown);
                window.addEventListener('keyup', handleKeyUp);
                return () => {
                        window.removeEventListener('keydown', handleKeyDown);
                        window.removeEventListener('keyup', handleKeyUp);
                }
        });
        wsRef.current = new WebSocket("ws://localhost:8080");
        wsRef.current.addEventListener('open', (event) => {
                console.log('web socket opened');
        });
        wsRef.current.addEventListener('message', (event) => {
                console.log(`SERVER: ${event.data}`);
        });
        return (
                <div>
                        <script src="game_canvas_script.ts" />
                        <canvas id="gameCanvas" width="600" height="400" ></canvas>
                </div>
        );
}
