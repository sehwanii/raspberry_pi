// 카메라 촬영 함수
function takePhoto() {
    fetch('/camera/takePhoto')
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                document.getElementById('photoPreview').innerHTML = '<img src="' + data.imagePath + '" alt="촬영된 이미지">';
                playMelody(); // 이미지 저장 후 멜로디 재생
            } else {
                alert('촬영 실패');
            }
        });
}

// 멜로디 재생 함수
function playMelody() {
    fetch('/buzzer/playMelody')
        .then(response => response.json())
        .then(data => {
            if (!data.success) {
                alert('멜로디 재생 실패');
            }
        });
}

// 7세그먼트 표시 함수
function displayOnSevenSegment() {
    const number = document.getElementById('sevenSegmentNumber').value;
    if (number >= 0 && number <= 9) {
        fetch('/sevenSegment/display', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ number: number })
        }).then(response => response.json())
          .then(data => {
              if (!data.success) {
                  alert('7세그먼트 표시 실패');
              }
          });
    } else {
        alert('0에서 9까지의 숫자만 입력해주세요.');
    }
}

// LCD 표시 함수
function displayOnLCD() {
    const text = document.getElementById('lcdText').value;
    fetch('/lcd/display', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ text: text })
    }).then(response => response.json())
      .then(data => {
          if (!data.success) {
              alert('LCD 표시 실패');
          }
      });
}
