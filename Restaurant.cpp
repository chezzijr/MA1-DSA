#include "main.h"
#ifdef DEBUG
#define LOG(x) clog << x << endl
#else
#define LOG(x)
#endif

extern int MAXSIZE;

template<typename T>
void swap(T*& a, T*& b) {
	T* tmp = a;
	a = b;
	b = tmp;
}

class imp_res : public Restaurant
{
	private:
        class Queue {
        private:
			static int generateId() {
				static int id = 0;
				return id++;
			}
            friend class Table;
            friend class imp_res;

            int size;

			struct Node {
				customer* cus;
				Node* next;
				Node* prev;
			};
				
            Node* front;
            Node* back;


        public:
			Queue() : size(0) {}

			bool isFull() const {
				return size == MAXSIZE;
			}

			bool isEmpty() const {
				return size == 0;
			}


            void push(customer* cus) {
				// cyclic queue
				if (isFull()) return; 
				// LOG(cus);
				if (isEmpty()) {
					front = new Node {cus, nullptr, nullptr};
					back = front;
				} else {
					back->next = new Node {cus, nullptr, back};
					back = back->next;
				}
				size++;
            }

            void pop() {
				if (isEmpty()) return;

				if (size == 1) {
					front->cus = nullptr;
					delete front;
					front = nullptr;
					back = nullptr;
				} else {
					Node* temp = front;
					front = front->next;
					front->prev = nullptr;
					temp->next = nullptr;
					temp->cus = nullptr;
					delete temp;
				}
				size--;
            }


			bool checkExist(const string& name) const {
				if (isEmpty()) {
					return false;
				}
				Node* curr = front;
				while (curr != nullptr) {
					if (curr->cus->name == name) {
						return true;
					}
					curr = curr->next;
				}
				return false;
			}

			Node* getFront() const {
				return front;
			}

			Node* getBack() const {
				return back;
			}

            int getSize() const {
                return size;
            }

			Node* get(int index) const {
				if (index < 0 || index >= size) {
					throw out_of_range("index out of range");
				}
				Node* curr = front;
				for (int i = 0; i < index; i++) {
					curr = curr->next;
				}
				return curr;
			}

			void del(customer* cus) {
				if (isEmpty()) {
					return;
				}
				Node* curr = front;
				while (curr != nullptr) {
					if (curr->cus == cus) {
						if (curr == front) front = front->next;
						if (curr == back) back = back->prev;
						if (curr->prev != nullptr) curr->prev->next = curr->next;
						if (curr->next != nullptr) curr->next->prev = curr->prev;
						curr->cus = nullptr;
						delete curr;
						size--;
						return;
					}
					curr = curr->next;
				}
			}

			// return true if a came before b
			// false if a == b or b came before a or a or b not in queue
			bool cameBefore(customer* a, customer* b) {
				if (a == b) return false;
				Node* curr = front;
				while (curr != nullptr) {
					if (curr->cus == a) return true;
					if (curr->cus == b) return false;
					curr = curr->next;
				}
				return false;
			}

			int shellSort(Queue* queueTime) {
				if (size <= 1) return 0;
				int res = 0;
				int maxabs = 0;
				auto curr = front;
				customer* maxCus = curr->cus;
				for (int i = 0; i < size; i++) {
					int absEnergy = abs(curr->cus->energy);
					if (absEnergy > maxabs || (absEnergy == maxabs && !queueTime->cameBefore(curr->cus, maxCus))) {
						maxabs = abs(curr->cus->energy);
						res = i;
						maxCus = curr->cus;
					}
					curr = curr->next;
				}

				int sortSize = res + 1;
				int cnt = 0;

				auto inssort2 = [this, &cnt, queueTime] (int fw, int n, int incr) {
					for (int i = incr; i < n; i += incr) {
						for (int j = i + fw; j >= incr; j -= incr) {
							auto tmp1 = get(j);
							auto tmp2 = get(j - incr);
							int absE1 = abs(tmp1->cus->energy);
							int absE2 = abs(tmp2->cus->energy);
							if (absE1 > absE2 || (absE1 == absE2 && queueTime->cameBefore(tmp1->cus, tmp2->cus))) {
								swap(tmp1->cus, tmp2->cus);
								cnt++;
							} else {
								break;
							}
						}
					}
				};

				// shell sort
				for (int incr = sortSize / 2; incr > 2; incr /= 2) {
					for (int i = 0; i < incr; i++) {
						inssort2(i, sortSize - i, incr);
					}
				}
				inssort2(0, sortSize, 1);

				return cnt;
			}

			void print() const {
				if (isEmpty()) return;
				Node* curr = front;
				while (curr != nullptr) {
					curr->cus->print();
					curr = curr->next;
				}
			}

			void printRev() const {
				if (isEmpty()) return;
				Node* curr = back;
				while (curr != nullptr) {
					curr->cus->print();
					curr = curr->prev;
				}
			}

			int getSum() const {
				if (isEmpty()) return 0;
				int sum = 0;
				Node* curr = front;
				while (curr != nullptr) {
					sum += curr->cus->energy;
					curr = curr->next;
				}
				return sum;
			}

			Queue deleteHalf(bool isJ) {
				// filter all nodes with energy > 0 if isJ is true else energy < 0
				Queue keep;
				Queue discard;
				Node* curr = front;
				for (int i = 0; i < size; i++) {
					customer* cus = curr->cus;
					if ((isJ && cus->energy > 0) || (!isJ && cus->energy < 0)) {
						discard.push(cus);
					} else {
						keep.push(cus);
					}
					curr = curr->next;
				}
				*this = keep;
				return discard;
			}
        };

		class Table {
		private:
            friend class imp_res;
			int size;
			customer* latest;
			Queue* order;
		public:
			Table() : size(0), latest(nullptr) {
				order = new Queue();
			};

			~Table() {
				delete order;
			}

			bool isEmpty() const {
				return size == 0;
			}

			bool isFull() const {
				return size == MAXSIZE;
			}

			void add(customer* cus) {
				if (isEmpty()) {
					latest = cus;
					latest->next = latest;
                    latest->prev = latest;
				} 
				else if (size < MAXSIZE / 2) {
                    if (cus->energy >= latest->energy) {
                        addLatestNext(cus);
                    } else {
                        addLatestPrev(cus);
                    }
				} 
				else {
                    customer* curr = latest;
                    customer* res;
                    int maxdiff = 0;
                    do {
                        int absdiff = abs(cus->energy - curr->energy);
                        if (absdiff > maxdiff) {
                            maxdiff = absdiff;
                            res = curr;
                        }
						curr = curr->next;
                    } while (curr != latest);

					latest = res;
					if (cus->energy - res->energy < 0) {
						addLatestPrev(cus);
					} else {
						addLatestNext(cus);
					}
                }
				
				latest = cus;

				order->push(latest);
				size++;
			}

			void addLatestNext(customer* cus) {
				cus->prev = latest;
				cus->next = latest->next;
				latest->next->prev = cus;
				latest->next = cus;
			}

            void addLatestPrev(customer* cus) {
                cus->prev = latest->prev;
                latest->prev->next = cus;
                cus->next = latest;
                latest->prev = cus;
            }

			void deleteEarliest() {
				if (isEmpty()) {
					return;
				}
				if (size == 1) {
					order->pop();
					latest->prev = nullptr;
					latest->next = nullptr;
					delete latest;
					latest = nullptr;
					size--;
					return;
				}
				auto cus = order->getFront()->cus;
				order->pop();
				if (cus->energy > 0) latest = cus->next;
				else latest = cus->prev;

				cus->prev->next = cus->next;
				cus->next->prev = cus->prev;

				cus->prev = nullptr;
				cus->next = nullptr;
				delete cus;
				size--;
			}

			int getSum() const {
				if (isEmpty()) {
					return 0;
				}
				return order->getSum();
			}

			void print(bool clockwise) const {
				if (isEmpty()) {
					return;
				}
				customer* curr = latest;
				for (int i = 0; i < size; i++) {
					curr->print();
					curr = clockwise ? curr->next : curr->prev;
				}
			}

			Queue deleteHalf(bool isJ) {
				// filter all nodes with energy > 0 if isJ is true else energy < 0
				if (isEmpty()) {
					return Queue();
				}

				Queue discard;
				auto curr = order->getBack();
				while (curr != nullptr) {
					auto next = curr->prev;
					customer* cus = curr->cus;
					if ((isJ && cus->energy > 0) || (!isJ && cus->energy < 0)) {
						if (cus->energy > 0) latest = cus->next;
						else latest = cus->prev;

						cus->prev->next = cus->next;
						cus->next->prev = cus->prev;

						cus->prev = nullptr;
						cus->next = nullptr;
						discard.push(cus);
						
						if (curr == order->getFront()) order->front = order->front->next;
						if (curr == order->getBack()) order->back = order->back->prev;
						if (curr->prev != nullptr) curr->prev->next = curr->next;
						if (curr->next != nullptr) curr->next->prev = curr->prev;
						
						curr->prev = nullptr;
						curr->next = nullptr;
						curr->cus = nullptr;

						delete curr;
						order->size--;
						size--;
					}
					curr = next;
				}
				return discard;
			}

			void deleteAll() {
				if (size <= 0) {
					return;
				}
				// delete all customers
				while (size > 0) {
					deleteEarliest();
				}
			}

			void minSumMaxSubarray() const {
				if (size < 4) {
					return;
				}
				int start = size - 1, len = size;

				int minSum = getSum();
				customer* initial = latest;
				for (int i = 0; i < size; i++) {
					int currsum = 0;
					customer* curr = initial;
					// get sum of first 3 adjacent customers
					for (int j = 0; j < 3; j++) {
						currsum += curr->energy;
						curr = curr->next;
					}
					// get sum of next customers so that total length is greater or equal to 4
					for (int j = 0; j < size - 4; j++) {
						currsum += curr->energy;
						if (currsum < minSum || (currsum == minSum && j + 4 >= len)) {
							minSum = currsum;
							start = i;
							len = j + 4;
						}
						curr = curr->next;
					}
					initial = initial->next;
				}

				customer* curr = get(start);
				int minEnergy = curr->energy;
				int minElemIndex = 0;
				customer* minCus = curr;
				for (int i = 0; i < len; i++) {
					if (curr->energy < minEnergy) {
						minEnergy = curr->energy;
						minCus = curr;
						minElemIndex = i;
					}
					curr = curr->next;
				}

				curr = minCus;

				for (int i = minElemIndex; i < len; i++) {
					curr->print();
					curr = curr->next;
				}

				curr = get(start);

				for (int i = 0; i < minElemIndex; i++) {
					curr->print();
					curr = curr->next;
				}
			}

			customer* get(int index) const {
				if (index < 0 || index >= size) {
					throw out_of_range("index out of range");
				}
				customer* curr = latest;
				for (int i = 0; i < index; i++) {
					curr = curr->next;
				}
				return curr;
			}

			int getSize() const {
				return size;
			}

			
			bool isAdjacent(customer* a, customer* b) {
				if (a == nullptr || b == nullptr || a == b) return false;
				return a->next == b || a->prev == b || b->next == a || b->prev == a;
			}

			void reverseAll() {
				if (size <= 1) {
					return;
				}
				string name = latest->name;
				int energy = latest->energy;

				// to remain the order of customers after swapping information
				// string *ord = new string[order->getSize()];
				// auto cur = order->getFront();
				// for (int i = 0; i < order->getSize(); i++) {
				// 	ord[i] = cur->cus->name;
				// 	cur = cur->next;
				// }

				for (int isJ = 0; isJ < 2; isJ++) {
					// initialize
					int i = 1, j = size;
					customer* ci = latest->next, *cj = latest;
					while (i < j) {
						while (!((isJ && ci->energy > 0) || (!isJ && ci->energy < 0)) && i < j) {
							ci = ci->next;
							i++;
						}
						while (!((isJ && cj->energy > 0) || (!isJ && cj->energy < 0)) && i < j) {
							cj = cj->prev;
							j--;
						}
						if (i < j) {
							// the only time ci->prev = cj is when cj at latest and ci at latest->next
							// ... prev cj ci next ...
							if (ci == latest) latest = cj;
							else if (cj == latest) latest = ci;

							if (size > 2) {
								if (cj->next == ci || ci->prev == cj)
								{
									cj->prev->next = ci;
									ci->next->prev = cj;
									cj->next = ci->next;
									ci->prev = cj->prev;
									cj->prev = ci;
									ci->next = cj;
									swap(ci, cj);
								}
								// ... prev ci cj next ...
								else if (ci->next == cj || cj->prev == ci)
								{
									cj->next->prev = ci;
									ci->prev->next = cj;
									ci->next = cj->next;
									cj->prev = ci->prev;
									cj->next = ci;
									ci->prev = cj;
									swap(ci, cj);
								}
								else
								{
									swap(ci->next->prev, cj->next->prev);
									swap(ci->prev->next, cj->prev->next);
									swap(ci->prev, cj->prev);
									swap(ci->next, cj->next);
									swap(ci, cj);
								}
							}
							// swap(ci->name, cj->name);
							// swap(ci->energy, cj->energy);
							ci = ci->next;
							cj = cj->prev;
							i++;
							j--;
						};
					}
				}

				// restore order
				// customer* curr = latest;
				// for (int i = 0; i < order->getSize(); i++) {
				// 	while (curr->name != ord[i]) {
				// 		curr = curr->next;
				// 	}
				// 	order->pop();
				// 	order->push(curr);
				// }

				while (latest->name != name || latest->energy != energy) {
					latest = latest->next;
				}
			}
		};

		Table* table;

		// change entering time after shell sort
		Queue* queueOrder;

		// reserve entering time after shell sort
		Queue* queueTime;

	public:	
		imp_res() {
			table = new Table();
			queueOrder = new Queue();
			queueTime = new Queue();
		};

		~imp_res() {
			while (table->getSize() > 0) {
				table->deleteEarliest();
			}
			delete table;

			while (queueTime->getSize() > 0) {
				customer* cus = queueTime->getFront()->cus;
				queueTime->pop();
				queueOrder->del(cus);
				delete cus;
			}
			delete queueTime;
			delete queueOrder;
		}

		void RED(string name, int energy)
		{
			LOG("\033[1;31mRED:\033[0m " << name << " " << energy);
			if (energy == 0 || table->order->checkExist(name) || queueTime->checkExist(name)) {
				return;
			}
			customer *cus = new customer(name, energy, nullptr, nullptr);
			if (table->isFull()) {
				queueOrder->push(cus);
				queueTime->push(cus);
			} else {
				table->add(cus);
			}
		}
		void BLUE(int num)
		{
			LOG("\033[1;34mBLUE:\033[0m " << num);
            for (int i = 0; i < num && table->order->getSize() > 0; i++) {
                table->deleteEarliest();
            }

            for (int i = 0; i < num && queueOrder->getSize() > 0; i++) {
                customer* first = queueOrder->getFront()->cus;
				queueOrder->pop();
				queueTime->del(first);
				table->add(first);
            }
        }
		void PURPLE()
		{
			LOG("\033[1;35mPURPLE:\033[0m");
			int cnt = queueOrder->shellSort(queueTime);
			LOG("Number of swap: " << cnt);
			BLUE(cnt % MAXSIZE);
		}
		void REVERSAL()
		{
			LOG("\033[1;32mREVERSAL:\033[0m");
			table->reverseAll();
		}
		void UNLIMITED_VOID()
		{
			LOG("\033[1;7mUNLIMITED VOID:\033[0m");
			table->minSumMaxSubarray();
		}
		void DOMAIN_EXPANSION()
        {
			LOG("\033[1;33mDOMAIN EXPANSION:\033[0m");

			bool isJ = table->order->getSum() + queueTime->getSum() < 0;
			auto q1 = table->deleteHalf(isJ);
			queueOrder->deleteHalf(isJ);
			auto q2 = queueTime->deleteHalf(isJ);

			q2.printRev();
			q1.print();

			while (queueTime->getSize() > 0 && table->getSize() < MAXSIZE) {
				customer* cus = queueTime->getFront()->cus;
				queueTime->pop();
				queueOrder->del(cus);
				table->add(cus);
			}

		}

		void LIGHT(int num)
		{
			LOG("\033[1;37mLIGHT:\033[0m " << num);
            if (num != 0) {
				table->print(num > 0);
            } else {
                queueOrder->print();
            }
		}
};
