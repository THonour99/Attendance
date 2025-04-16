#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * 此程序用于生成与服务器模拟数据匹配的学生名单，
 * 可用于本地注册这些学生的人脸数据
 */
int main() {
    std::cout << "生成与模拟服务器匹配的学生数据...\n";
    
    // 确保数据目录存在
    fs::create_directories("../data");
    
    // 创建CSV文件，包含学生姓名和ID
    std::ofstream file("../data/students.csv");
    file << "姓名,学号,考场ID,座位号\n";
    
    // 为每个考场生成学生数据
    std::vector<int> examRoomIds = {1, 2, 3};
    std::vector<int> studentCounts = {30, 22, 15}; // 每个考场的学生数量
    
    int totalStudents = 0;
    
    for (size_t i = 0; i < examRoomIds.size(); ++i) {
        int examRoomId = examRoomIds[i];
        int count = studentCounts[i];
        
        std::cout << "为考场 " << examRoomId << " 生成 " << count << " 名学生...\n";
        
        for (int j = 1; j <= count; ++j) {
            std::string name = "学生" + std::to_string(examRoomId) + "-" + std::to_string(j);
            std::string studentId = "2023" + std::to_string(examRoomId) + std::string(3 - std::to_string(j).length(), '0') + std::to_string(j);
            
            file << name << "," << studentId << "," << examRoomId << "," << j << "\n";
            totalStudents++;
        }
    }
    
    file.close();
    
    std::cout << "已生成 " << totalStudents << " 名学生数据，保存在 ../data/students.csv\n";
    std::cout << "使用说明：\n";
    std::cout << "1. 在人脸识别系统中，使用上述CSV文件中的学生姓名和学号注册用户\n";
    std::cout << "2. 采集每个学生的人脸数据\n";
    std::cout << "3. 在考勤端登录时使用以下信息：\n";
    std::cout << "   - 服务器地址: http://localhost:8080\n";
    std::cout << "   - 用户名: operator\n";
    std::cout << "   - 密码: operator123\n";
    
    return 0;
} 