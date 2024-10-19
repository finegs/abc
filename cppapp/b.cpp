#include <cstdio>
#include <array>
#include <vector>
#include <iostream>
#include <chrono>
 
// 행렬을 1D로 처리한다.
// Compile 시간에 확정한다
template <typename T, int ROW, int COL>
class Matrix
{
public:
    Matrix() : mMatrix(ROW* COL, 0) {
        mRow = ROW;
        mCol= COL;
    };
 
    T& operator()(int rowIdx, int colIdx)
    {
        const int idx = rowIdx * COL + colIdx;
        return mMatrix[idx];
    };
private:
    std::vector<T> mMatrix;
    int mRow;
    int mCol;
};
 
int main()
{
    // C style
    int matrix[5][5]; // stack
    int** pMatrix = new int* [5];
    for (int i = 0; i < 5; i++)
    {
        pMatrix[i] = new int[5];
    }
 
    for (int i = 0; i < 5; i++)
    {
        delete[] pMatrix[i];
    }
    delete[] pMatrix;
 
    // C++ Array, Vector 이용
    std::array<std::array<int, 3>, 3> fixedMatrix;
    fixedMatrix[1][1] = 5;
    fixedMatrix[2][2] = 1;
 
    //alias template
    using N2dMatrix33 = std::array<std::array<int, 3>, 3>;
    N2dMatrix33 fixedMatrix2;
    /// <summary>
    /// Vector의 다차원 배열은 메모리 구조가 다르다. 
    /// 2*2 vector을 예로 설명을 하면
    /// stack에 pointer, capacity, size가 올라 가고
    /// pointer가 가르키는 벡터는 힙에 있다.
    /// 이 힙에 다시 pointer, capacity, size가 올라 가고
    /// 힙이 다시 vector 의 값을 가르키게 된다.
    /// </summary>
    /// <returns></returns>
    std::vector<std::vector<int>> dynamicMatrix(3, std::vector<int>(3));
    dynamicMatrix[0][2] = 10;
    dynamicMatrix[1][1] = 5;
    // 띄엄 띄엄 벡터가 위치하면 이미지 처리 같은 것을 할때
    // 성능상 손해다. 따라서 1D 형식으로 붙이는게 좋다.
 
    // fixedMatrix[1][1] = 5;
    // dynamicMatrix[0][2] = 10;
 
 
 
    Matrix<int,10,10> mat;
    mat(3, 3) = 3;
    mat(4, 3) = mat(3, 3) * 10;
    std::cout << mat(4, 3) << std::endl;
 
    // 다차원 Array의 Loop 돌리기
    // Cache 방향으로 inner loop를 잡아야 한다
    // Col -> Row 방향으로 Looping
    //std::array<std::array<int, 10>, 10> mat2;
    int msize = 3000;
    int csize = 3000;
    std::vector<std::vector<int>> mat2(msize, std::vector<int>(csize));
    auto start = std::chrono::high_resolution_clock::now();
    for (int rowIdx = 0; rowIdx < msize; rowIdx++)
    {
        for (int colIdx = 0; colIdx < csize; colIdx++)
        {
            mat2[rowIdx][colIdx] += 1;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "First col speed: " << diff.count() << std::endl;
    // Row -> Col 방향으로 inner looping
    start = std::chrono::high_resolution_clock::now();
    for (int colIdx = 0; colIdx < csize; colIdx++)
    {
        for (int rowIdx = 0; rowIdx < msize; rowIdx++)
        {
            mat2[rowIdx][colIdx] += 1;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "First row speed: " << diff.count() << std::endl;
    return 0;
 
}

