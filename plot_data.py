import sqlite3
import pandas as pd
import matplotlib.pyplot as plt

# 設定資料庫名稱
DB_NAME = 'sensor_data.db'

def plot_now():
    try:
        # 1. 從資料庫讀取資料
        with sqlite3.connect(DB_NAME) as conn:
            # 讀取整個表格
            df = pd.read_sql_query("SELECT * FROM measurements", conn)
        
        if df.empty:
            print("目前資料庫裡還沒有資料喔！")
            return

        # 2. 轉換時間格式
        df['timestamp'] = pd.to_datetime(df['timestamp'])

        # 3. 建立圖表視窗 (修改為 3 列子圖)
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12), sharex=True)

        # --- 畫距離圖 (Distance) ---
        ax1.plot(df['timestamp'], df['distance'], color='#1f77b4', label='Distance (cm)', linewidth=1.5)
        ax1.set_ylabel('Distance (cm)')
        ax1.set_title('Real-time Multi-Sensor Data Monitoring', fontsize=14)
        ax1.legend(loc='upper right')
        ax1.grid(True, linestyle='--', alpha=0.7)

        # --- 畫重量圖 (Weight) ---
        ax2.plot(df['timestamp'], df['weight'], color='#2ca02c', label='Weight (g)', linewidth=1.5)
        ax2.set_ylabel('Weight (g)')
        ax2.legend(loc='upper right')
        ax2.grid(True, linestyle='--', alpha=0.7)

        # --- 畫亮度圖 (Light) ---
        ax3.plot(df['timestamp'], df['light'], color='#ff7f0e', label='Light Intensity (%)', linewidth=1.5)
        ax3.set_ylabel('Light (%)')
        ax3.set_xlabel('Time')
        ax3.set_ylim(0, 105) # 亮度通常在 0-100 之間，固定座標軸較好觀察
        ax3.legend(loc='upper right')
        ax3.grid(True, linestyle='--', alpha=0.7)

        # 自動調整標籤角度與佈局
        plt.xticks(rotation=45)
        plt.tight_layout()
        
        print(f"成功讀取 {len(df)} 筆資料，正在顯示 3 軸傳感器圖表...")
        plt.show()

    except Exception as e:
        print(f"繪圖發生錯誤: {e}")

if __name__ == "__main__":
    plot_now()