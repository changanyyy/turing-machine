;我利用更相减损法，通过用大数减小数，迭代进行，当最后剩余两个数相等时候就是结果！
;程序开始的时候，先把第一行第一个数挪到第二行，然后把第二行读头放到第二行开头
;这时可以调用cmp逐个1进行比较，cmp遇到第一第二行都是1的时候，会把两个1都变成_，然后在第三行写入1
;在cmp一次之后，三个纸带的格局就是，第一第二纸带之一为空，另一个剩下的是减法之后的结果，第三个纸带上面是刚刚做减法两个数中比较小的那个
;这时根据空纸带的编号，进入321和322状态，他们的意思分别是：把3号纸带的数搬到1号纸带，把3号纸带的数搬到2号纸带
;当我搬完纸带，发现读头刚好在两个纸带的左端，这时可以继续调用cmp
;最后，当cmp读到00*的时候，说明算法结束，进入acc状态，把3号纸带的结果搬到1号纸带
;进入acc_halt，结束


; 有7个状态，在下面转移函数中，我会详细说明它们的作用
#Q = {0,1,cmp,321,322,acc,acc_halt}
 
; 其中输入符号只有0和1，对于表示一个数，几个1就表示几，中间用0分割，111011表示3和2
#S = {0,1}

; 输入符号集合加上空白符
#G = {0,1,_}

;初始状态是0
#q0 = 0

;空白符
#B = _

;最终停机接受状态，这时结果被放在第一条纸袋上
#F = {acc_halt}

;三条纸带，前两条用于比较，第三条起到缓冲区的作用
#N = 3

;状态0负责把第一行的第一个数搬到第二行
0 1** _1* rr* 0
;搬完要跳转到状态1，把第二行读头放在最左端
0 0** _** rl* 1

;把第二行读头放在最左端，方便cmp比较
1 *1* *1* *l* 1
1 *_* *_* *r* cmp

;这时前两纸带读头都在最左端，一个个进行比较，并且在第三行写下一个个1
cmp 11* __1 rrr cmp
;减法完成， 2号纸带剩下，需要把3号纸带内容移到1号纸带
cmp _1* _1* **l 321
;减法完成， 1号纸带剩下，需要把3号纸带内容移到2号纸带
cmp 1_* 1_* **l 322
;两个数相等，进入acc状态作最后处理
cmp __* __* **l acc

;这两个就是把第三行挪到上面，然后发现读头刚好在1，2纸带最左端，进入cmp
321 **1 1*_ l*l 321
321 **_ **_ r*r cmp
322 **1 *1_ *ll 322
322 **_ **_ *rr cmp

;acc会把第三行的结果移动到第一行，进入acc_halt结束！！！
acc **1 1*_ l*l acc
acc **_ **_ **r acc_halt