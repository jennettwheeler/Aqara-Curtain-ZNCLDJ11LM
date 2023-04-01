#include "esphome.h"

#define get_curtain(constructor) static_cast<AqaraCurtainComponent *>(constructor.get_component(0))

static uint16_t MODBUS_CRC16_v3( const unsigned char *buf, unsigned int len )
{
  static const uint16_t table[256] = {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

  uint8_t xor_ = 0;
  uint16_t crc = 0xFFFF;

  while( len-- )
  {
    xor_ = (*buf++) ^ crc;
    crc >>= 8;
    crc ^= table[xor_];
  }

  return crc;
}

class AqaraCurtainComponent : public Component, public UARTDevice, public Cover {
 public:
  AqaraCurtainComponent(UARTComponent *parent) : UARTDevice(parent) {}

  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(false);
    traits.set_supports_position(true);
    traits.set_supports_tilt(false);
    return traits;
  }

  void setup() override {
  }

  void loop() override {
    if (available()) {
      while (available()) {
        readByte(read());
      }
    } else if (!init_cal_known) {
      send_message({0x01, 0x09, 0x01}); // Request Calibration
    } else if (!init_dir_known) {
      send_message({0x01, 0x03, 0x01}); // Request Direction
    } else if (!init_status_known) {
      send_message({0x01, 0x05, 0x01}); // Request Status
    } else if (!init_pos_known) {
      send_message({0x01, 0x02, 0x01}); // Request Position
    }
  }

  void control(const CoverCall &call) override {
    if (call.get_stop()) {
      send_message({0x03, 0x03});
    }
    if (call.get_position().has_value()) {
      moving_due_to_sw_trigger = true;
      uint8_t pos = *call.get_position() * 100.0f;
      if (pos == 100) {
        send_message({0x03, 0x01});
      } else if (pos == 0) {
        send_message({0x03, 0x02});
      } else {
        send_message({0x03, 0x04, pos});
      }
    }
  }

  void send_message(const std::vector<uint8_t>& message) {
    std::vector<uint8_t> data = {0x55, 0xFE, 0xFE};
    data.reserve(data.size() + message.size() + 2);

    data.insert( data.end(), message.begin(), message.end());
    uint16_t crc = MODBUS_CRC16_v3(data.data(), data.size());
    data.push_back(crc & 0x00FFU);
    data.push_back((crc & 0xFF00U) >> 8U);

    write_array(data);
  }

  void set_status(uint8_t value)
  {
    switch (value) {
      case 0x01:
        current_operation = COVER_OPERATION_OPENING;
        if (!moving_due_to_sw_trigger) {
          is_manual->publish_state(true);
        }
        break;
      case 0x02:
        current_operation = COVER_OPERATION_CLOSING;
        if (!moving_due_to_sw_trigger) {
          is_manual->publish_state(true);
        }
        break;
      case 0x04:
        ESP_LOGW("curtain", "Obstruction!");
      case 0x03:
      case 0x00:
        is_manual->publish_state(false);
        current_operation = COVER_OPERATION_IDLE;
        break;
    }
    moving_due_to_sw_trigger = false;
  }

  void readByte(uint8_t data) {
    if ((offset == 0 && data == 0x55) || ((offset == 1 || offset == 2) && data == 0xFE) || offset > 2) {
      buffer[offset++] = data;
      if (offset > 6) {
        if (MODBUS_CRC16_v3(buffer, offset) == 0x0000) {
          if (offset >= 14 && buffer[3] == 0x04 && buffer[4] == 0x02 && buffer[5] == 0x08) {
            position = buffer[6] / 100.0f;
            direction_switch->update_state(buffer[7]);
            set_status(buffer[9]);
            cal_sensor->publish_state(buffer[13]);
          }
          if (offset >= 9 && buffer[3] == 0x01 && buffer[5] == 0x01) // Answers
          {
            switch (buffer[4]) {
              case 0x02: // position
                if (buffer[6] != 0xff)
                {
                  position = buffer[6] / 100.0f;
                }
                init_pos_known = true;
                break;
              case 0x03: // direction
                direction_switch->update_state(buffer[6]);
                init_dir_known = true;
                break;
              case 0x05: // status
                set_status(buffer[6]);
                init_status_known = true;
                break;
              case 0x09: // calibration
                cal_sensor->publish_state(buffer[6]);
                init_cal_known = true;
                break;
              default:
                break;
            }
          }
          offset = 0;
          publish_state();
        }
      }
    }
  }

  class AqaraCurtainDirection : public Component, public Switch {
  public:
    AqaraCurtainDirection(AqaraCurtainComponent* curtain) : curtain_(curtain) {
    }

    void setup() override {
    }
    
    void write_state(bool state) override {
      curtain_->send_message({0x02, 0x03, 0x01, state});
      update_state(state);
    }
    
    void update_state(bool state) {
      if (last_state_ == state) return;
      last_state_ = state;
      publish_state(state);
    }
    
  protected:
    AqaraCurtainComponent* curtain_;
    bool last_state_;
  };
  AqaraCurtainDirection* direction_switch = new AqaraCurtainDirection(this);
  
  BinarySensor* cal_sensor = new BinarySensor();
  class AqaraCurtainResetCalibration : public Component, public Switch {
  public:
    AqaraCurtainResetCalibration(AqaraCurtainComponent* curtain) : curtain_(curtain) {
    }

    void setup() override {
    }
    
    void write_state(bool state) override {
      curtain_->send_message({0x03, 0x07});
      curtain_->cal_sensor->publish_state(false);
      publish_state(false);
    }
    
  protected:
    AqaraCurtainComponent* curtain_;
  };
  AqaraCurtainResetCalibration* reset_cal_button = new AqaraCurtainResetCalibration(this);

  BinarySensor* is_manual = new BinarySensor();

protected:
  uint16_t offset = 0;
  uint8_t buffer[1024];
  bool init_cal_known = false;
  bool init_pos_known = false;
  bool init_dir_known = false;
  bool init_status_known = false;
  bool moving_due_to_sw_trigger = false;
};

