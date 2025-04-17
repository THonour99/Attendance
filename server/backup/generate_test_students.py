#!/usr/bin/env python3
import os
import csv

def main():
    """
    生成与模拟服务器匹配的学生数据，可用于本地注册这些学生的人脸数据
    """
    print("生成与模拟服务器匹配的学生数据...")
    
    # 确保数据目录存在
    data_dir = os.path.join("..", "data")
    os.makedirs(data_dir, exist_ok=True)
    
    # 创建CSV文件，包含学生姓名和ID
    csv_path = os.path.join(data_dir, "students.csv")
    
    # 为每个考场生成学生数据
    exam_room_ids = [1, 2, 3]
    student_counts = [30, 22, 15]  # 每个考场的学生数量
    
    total_students = 0
    
    with open(csv_path, 'w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["姓名", "学号", "考场ID", "座位号"])
        
        for i, (exam_room_id, count) in enumerate(zip(exam_room_ids, student_counts)):
            print(f"为考场 {exam_room_id} 生成 {count} 名学生...")
            
            for j in range(1, count + 1):
                name = f"学生{exam_room_id}-{j}"
                student_id = f"2023{exam_room_id}{j:03d}"
                
                writer.writerow([name, student_id, exam_room_id, j])
                total_students += 1
    
    print(f"已生成 {total_students} 名学生数据，保存在 {csv_path}")
    print("使用说明：")
    print("1. 在人脸识别系统中，使用上述CSV文件中的学生姓名和学号注册用户")
    print("2. 采集每个学生的人脸数据")
    print("3. 在考勤端登录时使用以下信息：")
    print("   - 服务器地址: http://localhost:8080")
    print("   - 用户名: operator")
    print("   - 密码: operator123")

if __name__ == "__main__":
    main() 