const SerialPort = require('serialport').SerialPort;
const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');

// 시리얼 포트 설정 (USB 포트 경로는 실제 환경에 맞게 변경)
const port = new SerialPort('COM3', {
    baudRate: 115200
});

// 시리얼 포트가 열릴 때
port.on('open', () => {
    console.log('Serial Port Opened');
});

// 시리얼 포트로부터 데이터가 들어올 때
port.on('data', (data) => {
    console.log('Data received:', data.toString());
});

// Express 앱 설정
const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// 정적 파일 제공 (HTML, CSS, JS 파일)
app.use(express.static('public'));

// 7세그먼트 디스플레이 요청 처리
app.post('/sevenSegment/display', (req, res) => {
    const number = req.body.number;
    console.log(`7 세그먼트에 ${number} 표시`);

    // 시리얼 포트를 통해 7세그먼트로 숫자 전송
    port.write(`7SEG:${number}\n`, (err) => {
        if (err) {
            console.error('Error writing to serial port:', err.message);
            return res.json({ success: false, message: err.message });
        }
        res.json({ success: true });
    });
});

// LCD 텍스트 표시 요청 처리
app.post('/lcd/display', (req, res) => {
    const text = req.body.text;
    console.log(`LCD에 "${text}" 표시`);

    // 시리얼 포트를 통해 LCD로 텍스트 전송
    port.write(`LCD:${text}\n`, (err) => {
        if (err) {
            console.error('Error writing to serial port:', err.message);
            return res.json({ success: false, message: err.message });
        }
        res.json({ success: true });
    });
});

// 카메라 촬영 요청 처리 (카메라 기능은 실제 구현에 맞게 추가 필요)
app.get('/camera/takePhoto', (req, res) => {
    console.log('카메라 촬영 요청 수신');
    const imagePath = '/images/sample-photo.jpg';  // 예제 이미지 파일 경로
    res.json({ success: true, imagePath });
});

// 부저 멜로디 재생 요청 처리
app.get('/buzzer/playMelody', (req, res) => {
    console.log('부저 멜로디 재생 요청 수신');

    // 시리얼 포트를 통해 부저 제어
    port.write('BUZZER:PLAY\n', (err) => {
        if (err) {
            console.error('Error writing to serial port:', err.message);
            return res.json({ success: false, message: err.message });
        }
        res.json({ success: true });
    });
});

// 서버 시작
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`서버가 포트 ${PORT}에서 실행 중입니다.`);
});
