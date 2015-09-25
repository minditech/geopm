/*
 * Copyright (c) 2015, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string>
#include <inttypes.h>
#include <cpuid.h>

#include "PlatformFactory.hpp"
#include "IVTPlatform.hpp"
#include "IVTPlatformImp.hpp"

namespace geopm
{

    PlatformFactory::PlatformFactory()
    {
        //register all the platforms we know about
//        std::unique_ptr<Platform> ivb_plat(new IVTPlatform());
//        std::unique_ptr<PlatformImp> ivb_plat_imp(new IVTPlatformImp());
//        register_platform(move(ivb_plat), move(ivb_plat_imp));
//        register_platform(std::unique_ptr<geopm::Platform>(new IVTPlatform()),
//                          std::unique_ptr<geopm::PlatformImp>(new IVTPlatformImp()));

//        ivb_plat->release();
//        ivb_plat_imp->release();
    }

    PlatformFactory::~PlatformFactory()
    {
        for (auto it = platforms.rbegin(); it != platforms.rend(); ++it) {
            delete it->first;
            delete it->second;
        }
    }

    Platform& PlatformFactory::platform(int level)
    {
        int platform_id;
        Platform *result = NULL;
        if (level == 0) {
            platform_id = read_cpuid();
            for (auto it = platforms.begin(); it != platforms.end(); ++it) {
                if (it->second != NULL &&
                    it->first->model_supported(platform_id) &&
                    it->second->model_supported(platform_id)) {
                    it->first->set_implementation(it->second);
                    result =  it->first;
                }
            }
        }
        else {
            for (auto it = platforms.begin(); it != platforms.end(); ++it) {
                if (it->first->level() == level) {
                    result = it->first;
                }
            }
        }
        if (!result) {
            // If we get here, no acceptable platform was found
            throw std::invalid_argument("no plugin found to support current platform");
        }

        return *result;
    }

    void PlatformFactory::register_platform(std::unique_ptr<Platform> platform, std::unique_ptr<PlatformImp> platform_imp)
    {
        platforms.insert(std::pair<Platform*, PlatformImp*>(platform.release(), platform_imp.release()));
    }

    int PlatformFactory::read_cpuid()
    {
        uint32_t key = 1; //processor features
        uint32_t proc_info = 0;
        uint32_t model;
        uint32_t family;
        uint32_t ext_model;
        uint32_t ext_family;
        uint32_t ebx, ecx, edx;
        const uint32_t model_mask = 0xF0;
        const uint32_t family_mask = 0xF00;
        const uint32_t extended_model_mask = 0xF0000;
        const uint32_t extended_family_mask = 0xFF00000;

        //Not sure if this is the correct call
        __get_cpuid(key, &proc_info, &ebx, &ecx, &edx);
        //Commenting out assembly which works only on x86_64
        //__asm__("cpuid"
        //        :"=a"(proc_info)
        //        :"0"(key)
        //        :"%ebx","%ecx","%edx");

        model = (proc_info & model_mask) >> 4;
        family = (proc_info & family_mask) >> 8;
        ext_model = (proc_info & extended_model_mask) >> 16;
        ext_family = (proc_info & extended_family_mask)>> 20;

        if (family == 6) {
            model+=(ext_model << 4);
        }
        else if (family == 15) {
            model+=(ext_model << 4);
            family+=ext_family;
        }

        return ((family << 8) + model);
    }
}