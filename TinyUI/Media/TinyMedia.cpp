#include "../stdafx.h"
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		AudioObserver::AudioObserver()
		{

		}
		AudioObserver::~AudioObserver()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		TinyBufferArray<T>::TinyBufferArray()
			:m_value(NULL),
			m_size(0)
		{

		}
		template<class T>
		TinyBufferArray<T>::TinyBufferArray(T* value, INT size)
			: m_value(value),
			m_size(size)
		{

		}
		template<class T>
		BOOL TinyBufferArray<T>::Add(T* value, INT size)
		{

		}
		template<class T>
		BOOL TinyBufferArray<T>::Insert(INT offset, T* value, INT size)
		{
			if (value == NULL || size == 0)
				return TRUE;
			if (offset > m_size)
				return FALSE;
			if (SetSize(size + m_size))
			{
				memmove_s((void*)(m_value + offset + size),
					(m_size - offset) * sizeof(T),
					(void*)(m_value + offset),
					(m_size - offset) * sizeof(T));
				memcpy(m_value + offset, value, sizeof(T)*size);
				return TRUE;
			}
			return FALSE;

		}
		template<class T>
		void TinyBufferArray<T>::Clear()
		{
			SAFE_DELETE_ARRAY(m_value);
			m_size = 0;
		}
		template<class T>
		BOOL TinyBufferArray<T>::SetSize(INT size)
		{
			if (m_size >= size)
			{
				return FALSE;
			}
			if (size == 0)
			{
				SAFE_DELETE_ARRAY(m_value);
				m_size = 0;
				return TRUE;
			}
			m_value = (T*)realloc(m_value, sizeof(T)*size);
			memset(&m_value[m_size], 0, size - m_size);
			m_size = size;
			return TRUE;
		}
		template<class T>
		TinyBufferArray<T>::~TinyBufferArray()
		{
			Clear();
		}
	};
}
