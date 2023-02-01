#pragma once

NAMESPACE_WF_S

template <typename T>
class UploadBuffer
{
public:
	UploadBuffer( ID3D12Device* _device, UINT _element_count, bool _is_constant_buffer )
		: m_is_constant_buffer( _is_constant_buffer )
	{
		if ( m_is_constant_buffer )
		{
			m_element_byte_size = utility::CalculateConstantBufferByteSize( sizeof( T ) );
		}
		else
		{
			m_element_byte_size = sizeof( T );
		}

		CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer( m_element_byte_size * _element_count );

		CHECK_HR( _device->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS( &m_buffer )
		) );

		CHECK_HR( m_buffer->Map( 0, nullptr, reinterpret_cast<void**>( &m_mapped_data ) ) );
	}

	UploadBuffer( const UploadBuffer& rhs ) = delete;
	UploadBuffer& operator=( const UploadBuffer& rhs ) = delete;

	~UploadBuffer()
	{
		if ( !m_buffer )
		{
			m_buffer->Unmap( 0, nullptr );
		}

		SAFE_RELEASE( m_buffer );

		m_mapped_data = nullptr;
	}

	ID3D12Resource* GetResource() const
	{
		return m_buffer;
	}
	
	void CopyData( int _element_index, const T& data )
	{
		memcpy_s( &m_mapped_data[ _element_index * m_element_byte_size ], sizeof( T ), &data, sizeof( T ) );
	}

private:
	ID3D12Resource* m_buffer{ nullptr };
	BYTE* m_mapped_data{ nullptr };
	UINT m_element_byte_size{ 0 };
	bool m_is_constant_buffer{ false };
};

NAMESPACE_WF_E
