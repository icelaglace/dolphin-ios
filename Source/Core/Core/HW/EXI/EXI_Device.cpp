// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Core/HW/EXI/EXI_Device.h"

#include <memory>

#include "Common/CommonTypes.h"
#include "Core/HW/EXI/EXI_DeviceAD16.h"
#include "Core/HW/EXI/EXI_DeviceAGP.h"
#include "Core/HW/EXI/EXI_DeviceDummy.h"
#include "Core/HW/EXI/EXI_DeviceEthernet.h"
#include "Core/HW/EXI/EXI_DeviceGecko.h"
#include "Core/HW/EXI/EXI_DeviceIPL.h"
#include "Core/HW/EXI/EXI_DeviceMemoryCard.h"
#include "Core/HW/EXI/EXI_DeviceMic.h"
#include "Core/HW/Memmap.h"
#include "Core/System.h"

namespace ExpansionInterface
{
void IEXIDevice::ImmWrite(u32 data, u32 size)
{
  while (size--)
  {
    u8 byte = data >> 24;
    TransferByte(byte);
    data <<= 8;
  }
}

u32 IEXIDevice::ImmRead(u32 size)
{
  u32 result = 0;
  u32 position = 0;
  while (size--)
  {
    u8 byte = 0;
    TransferByte(byte);
    result |= byte << (24 - (position++ * 8));
  }
  return result;
}

void IEXIDevice::ImmReadWrite(u32& data, u32 size)
{
}

void IEXIDevice::DMAWrite(u32 address, u32 size)
{
  auto& system = Core::System::GetInstance();
  auto& memory = system.GetMemory();
  while (size--)
  {
    u8 byte = memory.Read_U8(address++);
    TransferByte(byte);
  }
}

void IEXIDevice::DMARead(u32 address, u32 size)
{
  auto& system = Core::System::GetInstance();
  auto& memory = system.GetMemory();
  while (size--)
  {
    u8 byte = 0;
    TransferByte(byte);
    memory.Write_U8(byte, address++);
  }
}

bool IEXIDevice::UseDelayedTransferCompletion() const
{
  return false;
}

bool IEXIDevice::IsPresent() const
{
  return false;
}

void IEXIDevice::SetCS(int cs)
{
}

void IEXIDevice::DoState(PointerWrap& p)
{
}

void IEXIDevice::PauseAndLock(bool do_lock, bool resume_on_unlock)
{
}

bool IEXIDevice::IsInterruptSet()
{
  return false;
}

void IEXIDevice::TransferByte(u8& byte)
{
}

// F A C T O R Y
std::unique_ptr<IEXIDevice> EXIDevice_Create(const EXIDeviceType device_type, const int channel_num,
                                             const Memcard::HeaderData& memcard_header_data)
{
  std::unique_ptr<IEXIDevice> result;
  // XXX This computation isn't necessarily right (it holds for A/B, but not SP1)
  // However, the devices that care about slots currently only go in A/B.
  const Slot slot = static_cast<Slot>(channel_num);

  switch (device_type)
  {
  case EXIDeviceType::Dummy:
    result = std::make_unique<CEXIDummy>("Dummy");
    break;

  case EXIDeviceType::MemoryCard:
  case EXIDeviceType::MemoryCardFolder:
  {
    bool gci_folder = (device_type == EXIDeviceType::MemoryCardFolder);
    result = std::make_unique<CEXIMemoryCard>(slot, gci_folder, memcard_header_data);
    break;
  }
  case EXIDeviceType::MaskROM:
    result = std::make_unique<CEXIIPL>();
    break;

  case EXIDeviceType::AD16:
    result = std::make_unique<CEXIAD16>();
    break;

#ifdef HAVE_CUBEB
  case EXIDeviceType::Microphone:
    result = std::make_unique<CEXIMic>(channel_num);
    break;
#endif

  case EXIDeviceType::Ethernet:
    result = std::make_unique<CEXIETHERNET>(BBADeviceType::TAP);
    break;

#if defined(__APPLE__)
  case EXIDeviceType::EthernetTapServer:
    result = std::make_unique<CEXIETHERNET>(BBADeviceType::TAPSERVER);
    break;
#endif

  case EXIDeviceType::EthernetXLink:
    result = std::make_unique<CEXIETHERNET>(BBADeviceType::XLINK);
    break;

  case EXIDeviceType::EthernetBuiltIn:
    result = std::make_unique<CEXIETHERNET>(BBADeviceType::BuiltIn);
    break;

  case EXIDeviceType::Gecko:
    result = std::make_unique<CEXIGecko>();
    break;

  case EXIDeviceType::AGP:
    result = std::make_unique<CEXIAgp>(slot);
    break;

  case EXIDeviceType::AMBaseboard:
  case EXIDeviceType::None:
  default:
    result = std::make_unique<IEXIDevice>();
    break;
  }

  if (result != nullptr)
    result->m_device_type = device_type;

  return result;
}
}  // namespace ExpansionInterface
