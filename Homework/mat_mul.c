int mat_mul(int** src1, int** src2, int** dst, int len)
{
    int i,j,k;
    int result=0;
    for(i=0;i<len;i++){ //i(Row)
        for(j=0;j<len;j++){ //j(Cloumn)
            result = 0;
            for(k=0; k< len; k++){
                result += src1[i][k] * src2[k][j];
            }
            dst[i][j] = result;
        }
    }
    return 0;
}