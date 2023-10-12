// Copyright 2018 Isaac Hsu.

#pragma once

#include "Containers/Array.h"

template <typename ContainerType, typename ElementType>
class TCyclicBufferIteratorBase
{
	typedef TCyclicBufferIteratorBase ThisClass;

public:

	TCyclicBufferIteratorBase(ContainerType& InContainer, int32 InIndex)
		: Container(InContainer)
		, Index(InIndex)
	{}

	/** conversion to "bool" returning true if the iterator has not reached the last element. */
	FORCEINLINE explicit operator bool() const
	{
		return Container.IsValidIndex(Index);
	}
	/** inverse of the "bool" operator */
	FORCEINLINE bool operator!() const
	{
		return !(bool)*this;
	}

	/** @name Element access */
	//@{
	ElementType& operator*() const
	{
		return Container[Index];
	}

	ElementType* operator->() const
	{
		return &Container[Index];
	}
	//@}

protected:

	void Advance()
	{
		if (Index == Container.TailIndex) // If we have reached the tail
		{
			Index = Container.Num();
		}
		else
		{
			++Index;
			int32 Limit = Container.Num() - 1;
			if (Index > Limit && Container.TailIndex != Limit) // If we have reached the upper bound but not the tail
			{
				Index = 0;
			}
		}
	}

	void Recede()
	{
		if (Index == Container.GetHeadIndex()) // If we have reached the head
		{
			Index = INDEX_NONE;
		}
		else
		{
			--Index;
			int32 Limit = Container.Num() - 1;
			if (Index < 0 && Container.TailIndex != Limit) // If we have reached the lower bound but not the head
			{
				Index = Limit;
			}
		}
	}

	ContainerType& Container;
    int32 Index;
};

template <typename ContainerType, typename ElementType>
class TCyclicBufferIterator : public TCyclicBufferIteratorBase<ContainerType, ElementType>
{
	typedef TCyclicBufferIterator ThisClass;
	typedef TCyclicBufferIteratorBase<ContainerType, ElementType> Super;

public:

	TCyclicBufferIterator(ContainerType& InContainer)
		: Super(InContainer, InContainer.GetHeadIndex())
	{
	}

	TCyclicBufferIterator(ContainerType& InContainer, int32 StartIndex)
		: Super(InContainer, InContainer.GetHeadIndex() + StartIndex)
	{
		if (Super::Container.IsValidIndex(StartIndex))
		{
			if (Super::Index >= Super::Container.Num())
			{
				Super::Index -= Super::Container.Num();
			}
		}
		else
		{
			Super::Index = Super::Container.Num();
		}
	}

	/** Advances iterator to the next element in the container. */
	ThisClass& operator++()
	{
		Super::Advance();
		return *this;
	}
	ThisClass operator++(int)
	{
		ThisClass Tmp(*this);
		Super::Advance();
		return Tmp;
	}

	/** Moves iterator to the previous element in the container. */
	ThisClass& operator--()
	{
		Super::Recede();
		return *this;
	}
	ThisClass operator--(int)
	{
		ThisClass Tmp(*this);
		Super::Recede();
		return Tmp;
	}

	FORCEINLINE ThisClass& operator=(const ThisClass& RHS)
	{
		check(&Super::Container == &RHS.Container);

		Super::Index = RHS.Index;
		return *this;
	}
};

template <typename ContainerType, typename ElementType>
class TCyclicBufferReverseIterator : public TCyclicBufferIteratorBase<ContainerType, ElementType>
{
	typedef TCyclicBufferReverseIterator ThisClass;
	typedef TCyclicBufferIteratorBase<ContainerType, ElementType> Super;

public:

	TCyclicBufferReverseIterator(ContainerType& InContainer)
		: Super(InContainer, InContainer.TailIndex)
	{}

	TCyclicBufferReverseIterator(ContainerType& InContainer, int32 StartIndex)
		: Super(InContainer, InContainer.TailIndex - StartIndex)
	{
		if (Super::Container.IsValidIndex(StartIndex))
		{
			if (Super::Index < 0)
			{
				Super::Index += Super::Container.Num();
			}
		}
		else
		{
			Super::Index = INDEX_NONE;
		}
	}

	/** Advances iterator to the next element in the container. */
	ThisClass& operator++()
	{
		Super::Recede();
		return *this;
	}
	ThisClass operator++(int)
	{
		ThisClass Tmp(*this);
		Super::Recede();
		return Tmp;
	}

	/** Moves iterator to the previous element in the container. */
	ThisClass& operator--()
	{
		Super::Advance();
		return *this;
	}
	ThisClass operator--(int)
	{
		ThisClass Tmp(*this);
		Super::Advance();
		return Tmp;
	}

	FORCEINLINE ThisClass& operator=(const ThisClass& RHS)
	{
		check(&Super::Container == &RHS.Container);

		Super::Index = RHS.Index;
		return *this;
	}
};

/**
* Homogeneous cyclic buffer based on TArray. When a new element is added, the oldest one may be replaced if the buffer is full.
*
* Caution: Must resize the buffer before adding elements to it.
*/
template <typename ElementType, typename Allocator = FDefaultAllocator>
class TCyclicBuffer : private TArray<ElementType, Allocator>
{
	typedef TArray<ElementType, Allocator> Super;

public:

	friend TCyclicBufferIteratorBase<TCyclicBuffer, ElementType>;
	friend TCyclicBufferIteratorBase<const TCyclicBuffer, const ElementType>;

	typedef TCyclicBufferIterator<TCyclicBuffer, ElementType> TIterator;
	friend TIterator;

	typedef TCyclicBufferIterator<const TCyclicBuffer, const ElementType> TConstIterator;
	friend TConstIterator;

	typedef TCyclicBufferReverseIterator<TCyclicBuffer, ElementType> TReverseIterator;
	friend TReverseIterator;

	typedef TCyclicBufferReverseIterator<const TCyclicBuffer, const ElementType> TConstReverseIterator;
	friend TConstReverseIterator;

public:

	TCyclicBuffer() : TailIndex(INDEX_NONE) {}

	using Super::Num;
	using Super::Max;
	using Super::GetData;

	/* Returns whether the buffer is full. */
	FORCEINLINE bool IsFull() const
	{
		return Super::ArrayNum == Super::ArrayMax;
	}

	/**
	* Returns n-th element from the buffer.
	*
	* @param Index (Optional) Index from the beginning of buffer (default = 0).
	* @returns Reference to n-th element from the buffer.
	*/
	FORCEINLINE ElementType* First(int32 Index = 0)
	{
		ElementType* Result = nullptr;
		if (Super::IsValidIndex(Index))
		{
			Index = InternalHeadIndex() + Index;
			if (Index >= Super::ArrayNum)
			{
				Index -= Super::ArrayNum;
			}
			Result = GetData() + Index;
		}
		return Result;
	}

	/**
	* Returns n-th element from the buffer.
	*
	* Const version of the above.
	*
	* @param Index (Optional) Index from the beginning of buffer (default = 0).
	* @returns Reference to n-th element from the buffer.
	*/
	FORCEINLINE const ElementType* First(int32 Index = 0) const
	{
		const ElementType* Result = nullptr;
		if (Super::IsValidIndex(Index))
		{
			Index = InternalHeadIndex() + Index;
			if (Index >= Super::ArrayNum)
			{
				Index -= Super::ArrayNum;
			}
			Result = GetData() + Index;
		}
		return Result;
	}

	/**
	* Returns n-th last element from the buffer.
	*
	* @param IndexFromTheEnd (Optional) Index from the end of buffer (default = 0).
	* @returns Reference to n-th last element from the buffer.
	*/
	FORCEINLINE ElementType* Last(int32 IndexFromTheEnd = 0)
	{
		ElementType* Result = nullptr;
		if (Super::IsValidIndex(IndexFromTheEnd))
		{
			int32 Index = TailIndex - IndexFromTheEnd;
			if (Index < 0)
			{
				Index += Super::ArrayNum;
			}
			Result = GetData() + Index;
		}
		return Result;
	}

	/**
	* Returns n-th last element from the buffer.
	*
	* Const version of the above.
	*
	* @param IndexFromTheEnd (Optional) Index from the end of buffer (default = 0).
	* @returns Reference to n-th last element from the buffer.
	*/
	FORCEINLINE const ElementType* Last(int32 IndexFromTheEnd = 0) const
	{
		const ElementType* Result = nullptr;
		if (IndexFromTheEnd >= 0 && IndexFromTheEnd < Super::ArrayNum)
		{
			int32 Index = TailIndex - IndexFromTheEnd;
			if (Index < 0)
			{
				Index += Super::ArrayNum;
			}
			Result = GetData() + Index;
		}
		return Result;
	}

	/**
	* Empties the array. It calls the destructors on held items if needed.
	*
	* @param Slack (Optional) The expected usage size after empty operation. Default is 0.
	*/
	FORCEINLINE void Reset(int32 Slack = 0)
	{
		Super::Reset(Slack);
		TailIndex = INDEX_NONE;
	}

	/**
	* Pushes a new item into the buffer, possibly replacing the oldest element if the buffer is full.
	*
	* @param Item The item to add
	* @return Index to the new item
	*/
	int32 Push(const ElementType& Item)
	{
		check(TailIndex < Super::ArrayMax);
		if (Super::ArrayNum < Super::ArrayMax)
		{
			TailIndex = Super::Add(Item);
		}
		else if (Super::ArrayMax == 0)
		{
			return INDEX_NONE;
		}
		else
		{
			int32 HeadIndex = InternalHeadIndex();
			(*this)[HeadIndex] = Item;
			TailIndex = HeadIndex;
		}

		return TailIndex;
	}

	/**
	* Adds a new item into the buffer, possibly extending the buffer if the buffer is full.
	*
	* @param Item The item to add
	* @return Index to the new item
	*/
	int32 Add(const ElementType& Item)
	{
		check(TailIndex < Super::ArrayMax);
		if (Super::ArrayNum < Super::ArrayMax || TailIndex == Super::ArrayMax - 1)
		{
			TailIndex = Super::Add(Item);
		}
		else
		{
			int32 HeadIndex = InternalHeadIndex();
			TailIndex = Super::Insert(Item, HeadIndex);
		}

		return TailIndex;
	}

	/**
	* Creates a iterator for the contents of this buffer
	*
	* @param StartIndex (Optional) Index from the beginning of buffer (default = 0).
	* @returns The iterator.
	*/
	TIterator CreateIterator(int32 StartIndex = 0)
	{
		return TIterator(*this, StartIndex);
	}

	/**
	* Creates a const iterator for the contents of this buffer
	*
	* @returns The const iterator.
	* @param StartIndex (Optional) Index from the beginning of buffer (default = 0).
	*/
	TConstIterator CreateConstIterator(int32 StartIndex = 0) const
	{
		return TConstIterator(*this, StartIndex);
	}

	/**
	* Creates a reverse iterator for the contents of this buffer
	*
	* @param StartIndex (Optional) Index from the end of buffer (default = 0).
	* @returns The iterator.
	*/
	TReverseIterator CreateReverseIterator(int32 StartIndex = 0)
	{
		return TReverseIterator(*this, StartIndex);
	}

	/**
	* Creates a const reverse iterator for the contents of this buffer
	*
	* @param StartIndex (Optional) Index from the end of buffer (default = 0).
	* @returns The const iterator.
	*/
	TConstReverseIterator CreateConstReverseIterator(int32 StartIndex = 0) const
	{
		return TConstReverseIterator(*this, StartIndex);
	}

protected:

	/* Returns the index of the first element if available. Otherwise, returns -1. */
	int32 GetHeadIndex() const
	{
		if (Super::ArrayNum == 0)
		{
			return INDEX_NONE;
		}
		//else if (Super::ArrayNum < Super::ArrayMax)
		//{
		//	return 0;
		//}
		else
		{
			return InternalHeadIndex();
		}
	}

	/* Returns the index of the first element. Note it returns 0 if there is no element in the buffer. */
	FORCEINLINE int32 InternalHeadIndex() const
	{
		check(Super::ArrayNum > 0);
		return (TailIndex < Super::ArrayNum - 1) ? TailIndex + 1 : 0;
	}

protected:

	/* The index of the last element. */
	int32 TailIndex;
};
