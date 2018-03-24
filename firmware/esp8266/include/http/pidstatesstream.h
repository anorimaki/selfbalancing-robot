#ifndef INCLUDE_HTTP_PIDSTATESSTREAM_H_
#define INCLUDE_HTTP_PIDSTATESSTREAM_H_

#include "motors_i2c_model.h"
#include <Stream.h>
#include <ArduinoJson.h>
#include <vector>
#include <memory>
#include <numeric>

namespace http
{

class PidStatesStream: public Stream {
private:
	class DummyPrint {
	public:
		size_t print(char) {
			return 1;
		}

		size_t print(const char* s) {
			return strlen(s);
		}
	};

public:
	PidStatesStream( std::unique_ptr<std::vector<::PIDStateEntry>> array ): m_array(std::move(array)) {
		m_length = calculateLength();
		m_read = 0;
		m_it = m_array->begin();
		m_currentSerialization = "[";
		m_currentSerializationPos = m_currentSerialization.begin();
	}

	int available() override {
		return m_length - m_read;
	}

	size_t write(uint8_t data) override {
		//Unsupported
		return 0;
	}

	int read() override {
		int ret = peek();
		if ( ret != -1 ) {
			++m_currentSerializationPos;
			++m_read;
		}
		return ret;
	}

	int peek() override {
		if ( m_currentSerializationPos == m_currentSerialization.end() ) {
			if ( m_it == m_array->end() ) {
				return -1;
			}
			m_currentSerialization = "";
			serialize(*m_it, m_currentSerialization);
			++m_it;

			char concatChar = (m_it == m_array->end()) ? ']' : ',';
			m_currentSerialization.concat(concatChar);
			m_currentSerializationPos = m_currentSerialization.begin();
		}

		return *m_currentSerializationPos;
	}

	void flush() override {}

private:
	int calculateLength() {
		int itemsSerializationSize = std::accumulate( m_array->begin(), m_array->end(), 0, [this](int acc, const ::PIDStateEntry& state) {
			DummyPrint print;
			return acc + serialize(state, print);
		});
		return itemsSerializationSize + 1 + m_array->size();
	}

	template <typename Print>
	size_t serialize( const ::PIDStateEntry& state, Print& print ) {
		StaticJsonBuffer<JSON_OBJECT_SIZE(5)> m_jsonBuffer;
		JsonObject& entry = m_jsonBuffer.createObject();
		entry["i"] = state.index;
		entry["tar"] = state.state.target;
		entry["cur"] = state.state.current;
		entry["p_err"] = state.state.previous_error;
		entry["i_err"] = state.state.integral_error;
		return entry.printTo(print);
	}

private:
	std::unique_ptr<std::vector<::PIDStateEntry>> m_array;
	int m_read;
	int m_length;
	std::vector<::PIDStateEntry>::const_iterator m_it;
	String m_currentSerialization;
	const char* m_currentSerializationPos;
};

}

#endif
