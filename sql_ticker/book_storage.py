import csv
import mysql.connector
from mysql.connector import Error

# 数据库连接信息
host = '127.0.0.1'
database = 'ticks'
user = 'root'
password = 'TJDX67537mcl.'

# CSV文件路径
csv_file_path = '20220801_trades.csv'

# 建立数据库连接
try:
    connection = mysql.connector.connect(host=host, database=database, user=user, password=password)

    # 打开CSV文件
    with open(csv_file_path, mode='r', encoding='utf-8') as csv_file:
        csv_reader = csv.reader(csv_file)
        header = next(csv_reader)  # 跳过标题行

        # 逐行处理CSV文件
        for row in csv_reader:
            # 构建插入语句
            insert_query = f"INSERT INTO 20220801_trades VALUES ({', '.join(['%s'] * len(row))})"
            
            # 执行插入语句
            cursor = connection.cursor()
            cursor.execute(insert_query, tuple(row))
            connection.commit()
            cursor.close()

    print("CSV文件已成功导入到数据库中。")

except Error as e:
    print("数据库错误：", e)
finally:
    if connection.is_connected():
        connection.close()
