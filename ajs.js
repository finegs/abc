// targetSum

const canSum = (targetSum, numbers, memo = {}) => {
	if(targetSum in memo) return memo[targetSum];
	if(targetSum === 0) return true;
	if(targetSum < 0) return false;

	for(let num of numbers) {
		const reminder = targetSum-num;
		if (canSum(reminder, numbers, memo) === true) {
			memo[targetSum] = true;
			return true;
		}
	}
	memo[targetSum] = false;
	return false;
};

console.log(canSum(7, [2,3]));
console.log(canSum(7, [5,4,3,7]));
console.log(canSum(7, [2,4]));
console.log(canSum(8, [2,3,5]));
console.log(canSum(300, [7,14]));


// gridTranveler : let start is top and left and end is  right and bottom.
// 					how many ways from start to end;
/*
const gridTranveler = (m, n, memo = {}) => {
	const key = m+','+n;
	if(key in memo) return memo[key];
	const key2 = n+','+m;
	if(key2 in memo) return memo[key2];

	if(key in memo) return memo[key];	
	if(m==1 && n==1) return 1;
	if(m==0 || n==0) return 0;

	memo[key]= gridTranveler(m-1,n, memo) + gridTranveler(m, n-1, memo);
	return memo[key];
};

console.log(gridTranveler(1,1));
console.log(gridTranveler(2,3));
console.log(gridTranveler(3,9));
console.log(gridTranveler(5,8));
console.log(gridTranveler(18,18));
*/

// fibonacci with memoizatio
/* fibonacci with memoizatio
const fib = (n, memo = {}) => {
	if(n in memo) return memo[n];
	if(n <=2) return 1;
	memo[n] = fib(n-1, memo) + fib(n-2, memo);
	return memo[n];
};

console.log(fib(6));
console.log(fib(100));
console.log(fib(200));
*/
