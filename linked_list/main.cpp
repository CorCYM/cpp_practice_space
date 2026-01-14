#include <iostream>
#include <string>

struct Node {
	int data;
	Node* next;

	Node(int value) : data(value), next(nullptr) {}
};

class SinglyLinkedList {
private:
	Node* head;
	Node* tail;
	std::size_t size;

public:
	SinglyLinkedList(): head(nullptr), tail(nullptr), size(0) {}
	~SinglyLinkedList() {
		clear();
	}

	void clear() {
		Node* cur = head;
		while (cur) {
			Node* next = cur->next;
			delete cur;
			cur = next;
		}
		head = tail = nullptr;
		size = 0;
	}

	void push_front(int value) {
		Node* node = new Node(value);
		node->next = head;
		head = node;
		if (tail == nullptr) {
			tail = node;
		}
		++size;
	}

	void push_back(int value) {
		Node* node = new Node(value);
		if (tail) {
			tail->next = node;
			tail = node;
		}
		else {
			head = tail = node;
		}
		++ size;
	}

	bool insert_at(std::size_t index, int value) {
		if (index > size) return false;

		if (index == 0) {
			push_front(value);
			return true;
		}
		if (index == size) {
			push_back(value);
			return true;
		}

		Node* prev = head;
		for (std::size_t i = 0; i < index - 1; ++i) {
			prev = prev->next;
		}

		Node* node = new Node(value);
		node->next = prev->next;
		prev->next = node;
		++size;
		
		return true;
	}

	bool erase_at(std::size_t index) {
		if (index >= size) return false;

		if (index == 0) {
			Node* del = head;
			head = head->next;
			if (del == tail) {
				tail = nullptr;
			}
			delete del;
			--size;
			return true;
		}

		Node* prev = head;
		for (std::size_t i = 0; i < index - 1; ++i) {
			prev = prev->next;
		}

		Node* del = prev->next;
		prev->next = del->next;

		if (del == tail) {
			tail = prev;
		}

		delete del;
		--size;

		return true;
	}

	Node* find(int value) const {
		Node* cur = head;
		std::size_t idx = 0;

		while (cur) {
			if (cur->data == value) {
				std::cout << "Value " << value << " found at index " << idx << "\n";
				return cur;
			}

			cur = cur->next;
			++idx;
		}

		std::cout << "Value" << value << " not found\n";
		return nullptr;
	}

	void print() const {
		Node* cur = head;
		std::cout << "[";
		while (cur) {
			std::cout << cur->data;
			if (cur->next) {
				std::cout << " -> ";
			}
			cur = cur->next;
		}
		std::cout << "]\n";
	}

	std::size_t get_size() const {
		return size;
	}

	SinglyLinkedList(const SinglyLinkedList&) = delete;
	SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;
};

void print_help() {
	std::cout << "===== Linked List Commands =====\n";
	std::cout << " push_front X   : insert X at front\n";
	std::cout << " push_back  X   : insert X at back\n";
	std::cout << " insert I X     : insert X at index I (0-based)\n";
	std::cout << " erase I        : erase node at index I\n";
	std::cout << " find X         : find value X\n";
	std::cout << " print          : print list\n";
	std::cout << " size           : print list size\n";
	std::cout << " clear          : clear list\n";
	std::cout << " help           : show this help\n";
	std::cout << " exit           : program exit\n";
	std::cout << "================================\n";
}

int main() {
	SinglyLinkedList list;
	std::string cmd;

	print_help();

	while (true) {
		std::cout << "> ";

		if (!(std::cin >> cmd)) {
			break;
		}

		if (cmd == "push_front") {
			int x;
			if (std::cin >> x) {
				list.push_front(x);
			}
			else {
				std::cout << "Invalid argument\n";
				break;
			}
		}
		else if (cmd == "push_back") {
			int x;
			if (std::cin >> x) {
				list.push_back(x);
			}
			else {
				std::cout << "Invalid argument\n";
				break;
			}
		}
		else if (cmd == "insert") {
			std::size_t idx;
			int x;
			if (std::cin >> idx >> x) {
				if (!list.insert_at(idx, x)) {
					std::cout << "Insert failed: index out of range\n";
				}
			}
			else {
				std::cout << "Invalid arguments\n";
				break;
			}
		}
		else if (cmd == "erase") {
			std::size_t idx;
			if (std::cin >> idx) {
				if (!list.erase_at(idx)) {
					std::cout << "Erase failed: index out of range\n";
				}
			}
			else {
				std::cout << "Invalid argument\n";
				break;
			}
		}
		else if (cmd == "find") {
			int x;
			if (std::cin >> x) {
				list.find(x);
			}
			else {
				std::cout << "Invalid arguments\n";
				break;
			}
		}
		else if (cmd == "print") {
			list.print();
		}
		else if (cmd == "size") {
			std::cout << "Size = " << list.get_size() << "\n";
		}
		else if (cmd == "clear") {
			list.clear();
			std::cout << "List cleared\n";
		}
		else if (cmd == "help") {
			print_help();
		}
		else if (cmd == "quit" || cmd == "exit") {
			break;
		}
		else {
			std::string rest;
			std::getline(std::cin, rest);
			std::cout << "Unknown command. Type 'help' for list.\n";
		}
	}

	std::cout << "Program exit \n";
	return 0;
}