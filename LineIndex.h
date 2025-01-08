#pragma once

namespace DCM
{
	class LineIndex
	{
	private:
		LineIndex* prev = nullptr;
		LineIndex* next = nullptr;
		int index;
		int length;
		int order;

		void updateIndex();
		void updateOrder();
		void update();
	public:
		LineIndex(int index = 0, int length = 1);
		LineIndex(LineIndex* _prev, LineIndex* _next = nullptr);
		int getIndex();
		int getEndIndex();
		int getOrder();
		void putLength(int len);
		void putEndIndex(int endIndex);
		void detachPrev(LineIndex* until = nullptr);
		void attachPrev(LineIndex* _prev);
		void detachNext(LineIndex* until = nullptr);
		void attachNext(LineIndex* _next);
		void attach(LineIndex* _prev, LineIndex* _next);
		void detach(LineIndex* _prev, LineIndex* _next);
		void independent();
		void resetIndex();
		LineIndex* findIndex(int line);
		LineIndex(const LineIndex& other);
		LineIndex& operator=(const LineIndex& other);
		~LineIndex();
	};

}