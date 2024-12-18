#include <iostream>
#include <sys/types.h>
#include <concepts>
#include <algorithm>
#include <cstdint>

template<class T>
requires std::totally_ordered<T>
class Set {
	struct Node {
		Node *parent = nullptr;
		T value;
		Node *left = nullptr, *right = nullptr;
		size_t height = 1;

		size_t left_height() const
		{
			return this->left ? this->left->height : 0;
		}

		size_t right_height() const
		{
			return this->right ? this->right->height : 0;
		}

		void update_height()
		{
			const auto lh = this->left_height();
			const auto rh = this->right_height();
			this->height = std::max(lh, rh) + 1;
		}

		ssize_t balance_factor() const
		{
			const auto lh = this->left_height();
			const auto rh = this->right_height();
			return static_cast<ssize_t>(rh - lh);
		}

		const Node &leftmost() const
		{
			auto root = this;
			while (root->left) root = root->left;
			return *root;
		}

		Node &leftmost()
		{
			auto root = this;
			while (root->left) root = root->left;
			return *root;
		}
	};

	static void delete_node_recursive(Node *node)
	{
		if (!node)
			return;
		if (node->left)
			delete_node_recursive(node->left);
		if (node->right)
			delete_node_recursive(node->right);
		delete node;
	}

	Node *root = nullptr;

	/// Возвращает изменяемую ссылку на указатель, соответствующий node.
	Node *&node_location(const Node &node)
	{
		if (!node.parent) return this->root;
		const auto left = node.parent->left == &node;
		return left ? node.parent->left : node.parent->right;
	}

	Node *find_node(const T &value)
	{
		auto node = this->root;
		while (node) {
			if (node->value == value) return node;
			node = value < node->value ? node->left : node->right;
		}
		return nullptr;
	}

	void rotate_left(Node &node)
	{
		Node &child = *node.right;
		auto t = child.left;

		node.right = t;
		node.update_height();
		if (t) t->parent = &node;

		child.left = &node;
		child.update_height();

		child.parent = node.parent;
		this->node_location(node) = &child;
		node.parent = &child;
	}

	void rotate_right(Node &node)
	{
		Node &child = *node.left;
		auto t = child.right;

		node.left = t;
		node.update_height();
		if (t) t->parent = &node;

		child.right = &node;
		child.update_height();

		child.parent = node.parent;
		this->node_location(node) = &child;
		node.parent = &child;
	}

	void rotate_right_left(Node &node)
	{
		this->rotate_right(*node.right);
		this->rotate_left(node);
	}

	void rotate_left_right(Node &node)
	{
		this->rotate_left(*node.left);
		this->rotate_right(node);
	}

	void rebalance(Node *node)
	{
		for (; node; node = node->parent) {
			node->update_height();
			const auto bf = node->balance_factor();
			if (bf >= 2) {
				const auto &right = *node->right;
				const auto right_bf = right.balance_factor();
				if (right_bf >= 0) {
					this->rotate_left(*node);
				} else {
					this->rotate_right_left(*node);
				}
				break;
			} else if (bf <= -2) {
				const auto &left = *node->left;
				const auto left_bf = left.balance_factor();
				if (left_bf <= 0) {
					this->rotate_right(*node);
				} else {
					this->rotate_left_right(*node);
				}
				break;
			}
		}
	}

	void erase_node(Node &node)
	{

		const uint8_t has_empty_childs = (uint8_t)!node.right << 1
		                                 | !node.left;
		auto parent = node.parent;
		switch (has_empty_childs) {
		case 0: {
			auto &right = *node.right;
			if (!right.left) {
				right.left = node.left;
				node.left->parent = &right;
				right.update_height();

				right.parent = parent;
				this->node_location(right) = right.right;
				this->node_location(node) = &right;
				break;
			}
			auto &leftmost = right.left->leftmost();
			this->node_location(leftmost) = leftmost.right;

			leftmost.left = node.left;
			node.left->parent = &leftmost;

			leftmost.right = node.right;
			node.right->parent = &leftmost;

			leftmost.update_height();
			std::swap(leftmost.parent, parent);
			this->node_location(node) = &leftmost;
			break;
		}
		case 0x1:
			node.right->parent = parent;
			this->node_location(node) = node.right;
			break;
		case 0x2:
			node.left->parent = parent;
			this->node_location(node) = node.left;
			break;
		case 0x3:
			this->node_location(node) = nullptr;
		}
		delete &node;
		this->rebalance(parent);
	}

public:
	Set()
	{
		std::cerr << "Создано множество\n";
	}

	~Set()
	{
		delete_node_recursive(this->root);
		std::cerr << "Уничтожено множество\n";
	}

	void insert(T value)
	{
		if (!this->root) {
			this->root = new Node { .value = value };
			return;
		}
		Node *parent = nullptr;
		Node *node = this->root;
		while (node) {
			parent = node;
			node = value < node->value ? node->left : node->right;
		}
		node = new Node { .parent = parent, .value = value };
		if (value < parent->value) {
			parent->left = node;
		} else {
			parent->right = node;
		}
		this->rebalance(parent);
	}

	void erase(const T &value)
	{
		const auto node = this->find_node(value);
		if (!node) return;
		this->erase_node(*node);
	}

	class Iterator {
		Set &parent;
		Node *root;

	public:
		Iterator(Set &parent, Node *root)
		:	parent(parent), root(root ? &root->leftmost() : nullptr)
		{}

		void erase()
		{
			this->parent.erase_node(*this->root);
		}

		bool operator!=(const Iterator &other)
		{
			return this->root != other.root;
		}

		T& operator*()
		{
			return this->root->value;
		}

		void operator++()
		{
			const auto right = this->root->right;
			if (right) {
				this->root = &right->leftmost();
				return;
			}

			while (const auto parent = this->root->parent) {
				const auto is_child_left = root == parent->left;
				if (is_child_left) {
					this->root = parent;
					return;
				}
				root = parent;
			}
			this->root = nullptr;
		}
	};

	Iterator begin()
	{
		return Iterator(*this, this->root);
	}

	Iterator end()
	{
		return Iterator(*this, nullptr);
	}
};
