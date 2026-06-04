import asyncio
import json
import time
import urllib.request
import urllib.parse
from bleak import BleakScanner, BleakClient

# ===== 設定區 =====
DEVICE_NAME     = "ESP32_SENSOR"
CHAR_UUID       = "0000ff01-0000-1000-8000-00805f9b34fb"
FIREBASE_URL    = "https://esp-to-web-72def-default-rtdb.asia-southeast1.firebasedatabase.app/sensor_data.json"
FIREBASE_SECRET = "FEUMMSOMzF3ERd3ko8kKrUjeDfeXWgKaFJ0LYzgH"
# ==================

def upload_firebase(distance, weight, light):
    data = {
        "distance":  distance,
        "weight":    weight,
        "light":     light,
        "timestamp": int(time.time())
    }
    url  = f"{FIREBASE_URL}?auth={FIREBASE_SECRET}"
    body = json.dumps(data).encode("utf-8")
    req  = urllib.request.Request(url, data=body, method="POST",
                                  headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=10) as resp:
        print(f"✅ 上傳成功 HTTP {resp.status} | {data}")

def handle_notify(sender, data):
    text = data.decode("utf-8", errors="replace").strip()
    print(f"📥 收到: {text}")
    try:
        parts = text.split(",")
        if len(parts) == 3:
            distance = float(parts[0])
            weight   = float(parts[1])
            light    = int(parts[2])
            upload_firebase(distance, weight, light)
    except Exception as e:
        print(f"❌ 解析失敗: {e}")

async def main():
    print(f"🔍 掃描 BLE 裝置: {DEVICE_NAME} ...")
    while True:
        try:
            device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10)
            if device is None:
                print("找不到裝置，重新掃描...")
                continue

            print(f"✅ 找到裝置: {device.address}")
            async with BleakClient(device) as client:
                print("✅ 已連線，開始接收資料...")
                await client.start_notify(CHAR_UUID, handle_notify)
                while client.is_connected:
                    await asyncio.sleep(1)
                print("⚠️ 裝置斷線，重新掃描...")

        except Exception as e:
            print(f"❌ 錯誤: {e}，3 秒後重試...")
            await asyncio.sleep(3)

if __name__ == "__main__":
    asyncio.run(main())
