import serial
import sqlite3
import time

# --- 設定區 ---
SERIAL_PORT = 'COM8'  
BAUD_RATE = 115200
DB_NAME = 'sensor_data.db'

def init_db():
    """初始化資料庫與表格，並確保欄位完整"""
    with sqlite3.connect(DB_NAME) as conn:
        cursor = conn.cursor()
        # 1. 建立表格（包含亮度 light 欄位）
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS measurements (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp DATETIME DEFAULT (datetime('now', 'localtime')),
                distance REAL,
                weight REAL,
                light INTEGER
            )
        ''')
        
        # 2. 檢查是否需要升級舊資料庫（萬一你之前已經產生了只有兩個欄位的 db）
        cursor.execute("PRAGMA table_info(measurements)")
        columns = [column[1] for column in cursor.fetchall()]
        if 'light' not in columns:
            print("檢測到舊版資料庫，正在升級欄位...")
            cursor.execute("ALTER TABLE measurements ADD COLUMN light INTEGER DEFAULT 0")
        
        conn.commit()
    print(f"資料庫 {DB_NAME} 初始化/更新成功。")

def main():
    init_db()
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"正在監聽 {SERIAL_PORT}...")
        
        # 清除掉剛開啟時快取區的雜訊
        ser.reset_input_buffer()
        
        while True:
            if ser.in_waiting > 0:
                try:
                    raw_line = ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    # 過濾系統資訊
                    if not raw_line or raw_line.startswith(('I (', 'W (', 'E (', 'rst:', 'load:')):
                        continue
                    
                    # 拆分數據：預期為 "距離,重量,亮度"
                    parts = raw_line.split(',')
                    
                    if len(parts) == 3:
                        dist = float(parts[0])
                        weight = float(parts[1])
                        light = int(parts[2])
                        
                        with sqlite3.connect(DB_NAME) as conn:
                            cursor = conn.cursor()
                            cursor.execute(
                                "INSERT INTO measurements (distance, weight, light) VALUES (?, ?, ?)",
                                (dist, weight, light)
                            )
                            conn.commit()
                        
                        print(f"成功存入 -> 距離: {dist:>6.2f} cm, 重量: {weight:>7.2f} g, 亮度: {light:>3d}%")
                    else:
                        # 處理格式不符的情況
                        if ',' in raw_line:
                            print(f"數據格式不完全(欄位數:{len(parts)}): {raw_line}")
                            
                except (ValueError, IndexError) as e:
                    print(f"解析錯誤: {raw_line} -> {e}")
                except Exception as e:
                    print(f"發生未預期錯誤: {e}")
                    
    except serial.SerialException as e:
        print(f"序列埠錯誤: {e}。請檢查 COM8 是否被其他程式（如 VS Code Monitor）佔用。")
    except KeyboardInterrupt:
        print("\n停止接收並關閉程式。")

if __name__ == "__main__":
    print("--- ESP32 數據採集系統啟動 ---")
    main()