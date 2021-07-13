#ifndef _OVERRIDEQUEUE
#define _OVERRIDEQUEUE

template<typename T>
class OverrideStack {
protected:
	std::vector<T*> arr;
	size_t start = 0; //points to the next free slot
	size_t len, currSize = 0;

public:
	OverrideStack(size_t size_) : arr(size_, nullptr), len(size_){

	}

	void push(const T& t) {
		arr[start] = new T(t);
		start = (start+1)%len;
		if (currSize < len) {
			currSize++;
		}
	}
	void push(const T* t) {
		arr[start] = t;
		start = (start+1)%len;
		if (currSize < len) {
			currSize++;
		}
	}
	bool pop(T* out) {
		if (!isEmpty()) {
			if (currSize > 0) {
				currSize--;
			}

			start = (start + len - 1) % len;
			if (out != NULL) {
				*out = *arr[start];
			}
			delete arr[start];
			arr[start] = nullptr;
			return true;
		}
		else {
			return false;
		}
	}
	T*& at(size_t ind) {
		int stackInd = (start + len - 1 - ind) % len;
		return arr[stackInd];
	}
	void clear() {
		for (size_t i = 0; i < len; i++) {
			if (arr[i] != nullptr) {
				delete arr[i];
				arr[i] = nullptr;
			}
		}
	}

	bool isEmpty() const{
		return currSize == 0;
	}
	size_t size() const{
		return currSize;
	}

	void resize(size_t newSize) {
		if (newSize == len)
			return;

		std::vector<T*> newVec(newSize, nullptr);

		if (newSize > len) {
			for (size_t i = 0; i < len; i++) {
				newVec[i] = at(len - 1 - i);
			}
			start = len;
		}
		else {
			for (size_t i = 0; i < newSize; i++) {
				newVec[newSize - 1 - i] = at(i);
			}
			for (size_t i = newSize; i < len; i++) {
				delete at(i);
			}
			start = 0;
		}
		
		len = newSize;
		arr = newVec;
	}
};

#endif