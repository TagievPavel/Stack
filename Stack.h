#ifndef STACK_H
#define STACK_H

#include <memory>
#include <initializer_list>
#include <type_traits>

template <typename Type>
class Stack
{
	static_assert(std::is_copy_constructible_v<Type>, "The copying constructor is not available");

public:
	Stack()  = default;
	~Stack() = default;

	Stack(Stack<Type>&& other) noexcept;
	Stack(const Stack<Type>& other);
	Stack(std::initializer_list<Type> list);


public:
	void push(const Type& data);
	void push(Type&& data);

	const Type& peek() const;
	std::size_t size() const noexcept;

	Type pop();

	bool isEmpty() const noexcept;

#ifndef NDEBUG
	void printChain();
#endif//NDEBUG

public:
	explicit operator bool() const noexcept;

	Stack<Type>& operator = (Stack<Type>&& other) noexcept;
	Stack<Type>& operator = (const Stack<Type>& other);

private:
	class Node
	{
	public:
		Node(std::unique_ptr<Node> next, const Type& data);
		Node(std::unique_ptr<Node> next, Type&& data);

		Node(const Node& other) = delete;
		Node(Node&&) = delete;

		~Node() = default;

	public:
		std::unique_ptr<Node> next;
		Type data;

	};

private:
	std::size_t _size;
	std::unique_ptr<Node> _top;

};

//inner class Node//
template<typename Type>
inline Stack<Type>::Node::Node(std::unique_ptr<Node> next, const Type& data):
	next(std::move(next)), data(data)
{

}

template<typename Type>
inline Stack<Type>::Node::Node(std::unique_ptr<Node> next, Type&& data):
	next(std::move(next)), data(std::move(data))
{

}

//class Stack//
template<typename Type>
inline Stack<Type>::Stack(Stack<Type>&& other) noexcept
{
	(*this) = std::move(other);
}

template<typename Type>
inline Stack<Type>::Stack(const Stack<Type>& other) : Stack()
{
	(*this) = other;
}

template<typename Type>
inline Stack<Type>::Stack(std::initializer_list<Type> list) : Stack()
{
	for (const auto& element : list)
		push(element);
}

template<typename Type>
inline void Stack<Type>::push(const Type& data)
{
	_top = std::make_unique<Node>(std::move(_top), data);
	++_size;
}

template<typename Type>
inline void Stack<Type>::push(Type&& data)
{
	_top = std::make_unique<Node>(std::move(_top), std::move(data));
	++_size;
}

template<typename Type>
inline const Type& Stack<Type>::peek() const
{
	return _top != nullptr ? _top->data : throw std::out_of_range("Out of range");
}

template<typename Type>
inline std::size_t Stack<Type>::size() const noexcept
{
	return _size;
}

template<typename Type>
inline Type Stack<Type>::pop()
{
	if (_top == nullptr) throw std::out_of_range("Out of range");

	auto data = std::move(_top->data);
	_top      = std::move(_top->next);
	--_size;

	return data;
}

template<typename Type>
inline bool Stack<Type>::isEmpty() const noexcept
{
	return !_size;
}

#ifndef NDEBUG
template<typename Type>
inline void Stack<Type>::printChain()
{
	for (auto curr = _top.get(); curr != nullptr; curr = curr->next.get())
		std::cout << curr->data << " -> ";
	std::cout << "nullptr\n"<<std::endl;
}
#endif//NDEBUG

template<typename Type>
inline Stack<Type>::operator bool() const noexcept
{
	return _size;
}

template<typename Type>
inline Stack<Type>& Stack<Type>::operator=(Stack<Type>&& other) noexcept
{
	if (this == &other) return *this;

	_top  = std::move(other._top);
	_size = other._size;
	other._size = 0;

	return *this;
}

template<typename Type>
inline Stack<Type>& Stack<Type>::operator=(const Stack<Type>& other)
{
	if (this == &other) return *this;

	bool empty = 
		(_top == nullptr) ? 
		(_top = std::make_unique<Node>(nullptr, other._top->data), true):
		(_top->data = other._top->data, false);

	auto thcr = this->_top.get();
	auto otcr = other._top->next.get();

	while (otcr != nullptr)
	{
		empty =
			(empty || thcr->next == nullptr) ?
			(thcr->next = std::make_unique<Node>(nullptr, otcr->data), true):
			(thcr->next->data = otcr->data, false);

		thcr = thcr->next.get();
		otcr = otcr->next.get();
	}

	thcr->next.release();
	_size =  other._size;

	return *this;

}

#endif//STACK_H