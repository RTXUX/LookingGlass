/*
KVMGFX Client - A KVM Client for VGA Passthrough
Copyright (C) 2017 Geoffrey McRae <geoff@hostfission.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#define W32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

#include "common\debug.h"
#include "ICapture.h"
#include "Capture\NvFBC.h"
#include "Capture\DXGI.h"

class CaptureFactory
{
public:
  typedef std::vector<ICapture *> DeviceList;

  static DeviceList & GetDevices()
  {
    static DeviceList devices;
    if (!devices.empty())
      return devices;

    devices.push_back(new Capture::NvFBC());
    devices.push_back(new Capture::DXGI ());

    return devices;
  }

  static ICapture * GetDevice(const char * name, CaptureOptions * options)
  {
    DeviceList devices = GetDevices();
    for (DeviceList::const_iterator it = devices.begin(); it != devices.end(); ++it)
    {
      ICapture * device = *it;
      if (_strcmpi(name, device->GetName()) != 0)
        continue;

      if (device->Initialize(options))
      {
        DEBUG_INFO("Using %s", device->GetName());
        return device;
      }

      device->DeInitialize();
      DEBUG_ERROR("Failed to initialize %s", device->GetName());
      return NULL;
    }

    DEBUG_ERROR("No such device: %s", name);
    return NULL;
  }

  static ICapture * DetectDevice(CaptureOptions * options)
  {
    DeviceList devices = GetDevices();
    for (DeviceList::const_iterator it = devices.begin(); it != devices.end(); ++it)
    {
      ICapture * device = *it;

      DEBUG_INFO("Trying %s", device->GetName());
      if (device->Initialize(options))
      {
        DEBUG_INFO("Using %s", device->GetName());
        return device;
      }
      device->DeInitialize();
    }

    DEBUG_ERROR("Failed to initialize a capture device");
    return NULL;
  }
};