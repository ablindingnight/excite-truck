// ISD: Excite Truck's SD card library over IOS /dev/sdio (0x800BAA8C-0x800BC538). No reference
// exists; the ISD_* and sdu* names come from its own messages, the rest are guesses. Functions
// that the linker stripped from ET are kept (and marked) because their strings sit in the pool.
#include <revolution/types.h>
#include <revolution/os.h>
#include <revolution/ipc.h>
#include <revolution/ipc/ipcclt.h>
#include <revolution/ipc/memory.h>
#include <cstring>

#define ISD_ERR ((s32)0xC1000000)          // generic failure
#define ISD_ERR_TIMEOUT ((s32)0xC1000003)  // command timed out (expected on deselect)
#define ISD_ERR_SDHC ((s32)0xC1000020)     // SDHC card, unsupported
#define ISD_ERR_NO_CALLBACK ((s32)0xC2100000)

#define ISD_CARD_MEMORY 0x10000

void* iosAlloc(IOSHeapId id, u32 size);

typedef s32 (*ISDCallback)(s32 result, void* arg);

typedef struct ISDDevice {
    // total size: 0x28
    IOSFd fd;       // offset 0x0
    u32 unk4[3];    // offset 0x4
    u32 rca;        // offset 0x10, relative card address from the reset response
    u32 unk14;      // offset 0x14
    u32 unk18;      // offset 0x18
    u32 unk1C;      // offset 0x1C
    u32 max_offset; // offset 0x20, 0 = no limit
    u32 state;      // offset 0x24 (0 closed, 1 selected, 2 standby)
} ISDDevice;

typedef struct ISDCommand {
    // total size: 0x24
    u32 cmd;        // offset 0x0
    u32 type;       // offset 0x4
    u32 resp_type;  // offset 0x8
    u32 arg;        // offset 0xC
    u32 blk_cnt;    // offset 0x10
    u32 blk_size;   // offset 0x14
    void* dma_addr; // offset 0x18
    u32 is_dma;     // offset 0x1C
    u32 pad;        // offset 0x20
} ISDCommand;

typedef struct ISDHCRequest {
    // total size: 0x18
    u32 reg;   // offset 0x0
    u32 unk4;  // offset 0x4
    u32 unk8;  // offset 0x8
    u32 size;  // offset 0xC
    u32 val;   // offset 0x10
    u32 unk14; // offset 0x14
} ISDHCRequest;

typedef struct ISDAsync {
    // total size: 0x1C
    ISDCallback cb;      // offset 0x0
    void* cb_arg;        // offset 0x4
    ISDDevice* dev;      // offset 0x8
    u32 cmd;             // offset 0xC
    u32 is_ioctl;        // offset 0x10
    u32* resp;           // offset 0x14
    ISDCommand* req;     // offset 0x18
} ISDAsync;

static OSMutex isd_mutex;
static char isd_dev_name[3][0x20] ATTRIBUTE_ALIGN(32);

IOSHeapId isd_heap = -1;

static void* isd_ipc_lo;
static IOSIoVector* isd_vec;
static ISDHCRequest* isd_hc_req;
static u32* isd_dma_resp;
static u32* isd_resp;
static u32* isd_reg_buf;
static volatile s32 isd_busy;
static u32 isd_card_type;

s32 sduDatabuswidth(ISDDevice* dev, u32 width);
s32 sduGetSCR(ISDDevice* dev, u32* buf);

static s32 __ISDCallback(s32 result, void* arg) {
    ISDAsync* ctx = (ISDAsync*)arg;

    switch (ctx->cmd) {
    case 4:
        if (result != 0) {
            if (result == ISD_ERR_SDHC) {
                OSReport("   +++ SDHC Card inserted; card is not supported (rc=%X)+++\n", result);
            } else {
                OSReport("   +++ SD RESET Failed: %d +++\n", result);
            }
            ctx->dev->rca = 0;
        } else if (ctx->dev != NULL) {
            ctx->dev->rca = *ctx->resp;
            isd_card_type = ISD_CARD_MEMORY;
            ctx->dev->state = 2;
        }
        isd_busy = 0;
        break;
    case 0x12:
    case 0x19:
    case 0x26:
        isd_busy = 0;
        break;
    }

    if (ctx->cb != NULL && result != ISD_ERR_NO_CALLBACK) {
        ctx->cb(result, ctx->cb_arg);
    }
    if (ctx->req != NULL) {
        iosFree(isd_heap, ctx->req);
    }
    iosFree(isd_heap, ctx);
    return result;
}

s32 ISD_GetHCRegister(ISDDevice* dev, u32 reg, u32* p_val, u32 size) {
    s32 rc;

    if (isd_heap < 0) {
        rc = ISD_ERR;
    } else {
        isd_hc_req->reg = reg;
        isd_hc_req->size = size;
        isd_hc_req->val = 0;
        if (p_val != NULL) {
            rc = IOS_Ioctl(dev->fd, 2, isd_hc_req, sizeof(ISDHCRequest), isd_reg_buf, 4);
            if (rc != 0) {
                OSReport("Read HC Register Failed\n");
                rc = ISD_ERR;
            } else {
                *p_val = *isd_reg_buf;
            }
        } else {
            OSReport("Read HC Register NULL ptr\n");
            rc = ISD_ERR;
        }
    }

    if (rc != 0) {
        OSReport("%s: exit rc is 0x%X\n", __FUNCTION__, rc);
    }
    return rc;
}

s32 ISD_SetHCRegister(ISDDevice* dev, u32 reg, u32* p_val, u32 size) {
    s32 rc;

    if (isd_heap < 0) {
        rc = ISD_ERR;
    } else {
        isd_hc_req->reg = reg;
        isd_hc_req->size = size;
        if (p_val != NULL) {
            isd_hc_req->val = *p_val;
            rc = IOS_Ioctl(dev->fd, 1, isd_hc_req, sizeof(ISDHCRequest), NULL, 0);
            if (rc != 0) {
                OSReport("Write HC Register Failed\n");
                rc = ISD_ERR;
            }
        } else {
            OSReport("Write HC Register NULL ptr\n");
            rc = ISD_ERR;
        }
    }

    if (rc != 0) {
        OSReport("%s: exit rc is 0x%X\n", __FUNCTION__, rc);
    }
    return rc;
}

s32 ISD_GetDeviceStatus(ISDDevice* dev, u32* p_status) {
    s32 rc = ISD_ERR;

    if (isd_heap >= 0 && p_status != NULL) {
        OSLockMutex(&isd_mutex);
        if (isd_busy != 0) {
            OSUnlockMutex(&isd_mutex);
        } else {
            isd_busy = 1;
            OSUnlockMutex(&isd_mutex);
            rc = IOS_Ioctl(dev->fd, 0xB, NULL, 0, isd_reg_buf, 4);
            if (rc != 0) {
                OSReport("Could not retrieve device status!\n");
                isd_busy = 0;
            } else {
                *p_status = *isd_reg_buf;
                isd_busy = 0;
            }
        }
    }

    if (rc != 0) {
        OSReport("%s: exit rc is 0x%X\n", __FUNCTION__, rc);
    }
    return rc;
}

// Stripped from ET; inlined into the block transfers.
s32 sduSetClock(IOSFd fd, u32 clock) {
    s32 rc;

    *isd_reg_buf = clock;
    rc = IOS_Ioctl(fd, 6, isd_reg_buf, 4, NULL, 0);
    if (rc != 0) {
        OSReport("Set Clock Failed\n");
        rc = ISD_ERR;
    }
    return rc;
}

static s32 __ISDSendCommandV(IOSFd fd, u32 cmd, u32 type, u32 resp_type, u32 arg, void* buf, u32 blk_cnt,
                             u32 blk_size, u32 is_dma, u32* resp, ISDCallback cb, void* cb_arg) {
    ISDAsync* ctx;
    u32 num_in;
    u32 num_out;
    ISDCommand* req;
    s32 rc;

    if (is_dma != 0) {
        num_in = 2;
        num_out = 1;
    } else {
        num_in = 1;
        num_out = 2;
    }

    req = (ISDCommand*)iosAlloc(isd_heap, 0x40);
    if (req == NULL) {
        OSReport("Not enough IPC heap for sdCmd (%d bytes)\n", 0x40);
        rc = -0x16;
    } else {
        req->cmd = cmd;
        req->type = type;
        req->resp_type = resp_type;
        req->arg = arg;
        req->dma_addr = buf;
        req->blk_cnt = blk_cnt;
        req->blk_size = blk_size;
        req->is_dma = 1;
        isd_vec[0].base = (u8*)req;
        isd_vec[0].length = sizeof(ISDCommand);
        isd_vec[1].base = (u8*)buf;
        isd_vec[1].length = blk_cnt * blk_size;
        isd_vec[2].base = (u8*)isd_resp;
        isd_vec[2].length = 0x10;

        if (cb != NULL) {
            ctx = (ISDAsync*)iosAlloc(isd_heap, sizeof(ISDAsync));
            if (ctx == NULL) {
                OSReport("Req: Alloc failed\n");
                rc = -0x16;
            } else {
                ctx->cb = cb;
                ctx->cb_arg = cb_arg;
                ctx->resp = resp;
                ctx->cmd = cmd;
                ctx->req = req;
                rc = IOS_IoctlvAsync(fd, 7, num_in, num_out, isd_vec, __ISDCallback, ctx);
                if (rc != 0) {
                    OSReport("IOS_IoctlvAsync returned error:(%x)\n", rc);
                }
            }
        } else {
            rc = IOS_Ioctlv(fd, 7, num_in, num_out, isd_vec);
            if (rc != 0) {
                OSReport("IOS_IoctlvAsync returned error:(%x)\n", rc);
            } else if (resp != NULL) {
                *resp = *isd_resp;
            }
        }
    }

    if ((cb == NULL || rc != 0) && req != NULL) {
        iosFree(isd_heap, req);
    }
    return rc;
}

static s32 __ISDSendCommand(IOSFd fd, u32 cmd, u32 type, u32 resp_type, u32 arg, void* buf, u32 blk_cnt,
                            u32 blk_size, u32 is_dma, u32* resp, ISDCallback cb, void* cb_arg) {
    ISDAsync* ctx;
    s32 rc;
    ISDCommand* req;

    req = (ISDCommand*)iosAlloc(isd_heap, 0x40);
    if (req == NULL) {
        OSReport("Not enough IPC heap for sdCmd (%d bytes)\n", 0x40);
        rc = -0x16;
    } else {
        req->cmd = cmd;
        req->type = type;
        req->resp_type = resp_type;
        req->arg = arg;
        req->dma_addr = buf;
        req->blk_cnt = blk_cnt;
        req->blk_size = blk_size;
        req->is_dma = is_dma;
        if (resp != NULL) {
            if (cb != NULL) {
                ctx = (ISDAsync*)iosAlloc(isd_heap, sizeof(ISDAsync));
                if (ctx == NULL) {
                    OSReport("Req: Alloc failed\n");
                    rc = -0x16;
                } else {
                    ctx->cb = cb;
                    ctx->cb_arg = cb_arg;
                    ctx->resp = resp;
                    ctx->cmd = cmd;
                    ctx->is_ioctl = 1;
                    ctx->req = req;
                    rc = IOS_IoctlAsync(fd, 7, req, sizeof(ISDCommand), isd_resp, 0x10, __ISDCallback, ctx);
                    if (rc != 0) {
                        OSReport("IOS_IoctlAsync returned error:(%x)\n", rc);
                        rc = ISD_ERR;
                    }
                }
            } else {
                rc = IOS_Ioctl(fd, 7, req, sizeof(ISDCommand), isd_resp, 0x10);
                if (rc != 0) {
                    if (cmd == 7) {
                        rc = 0;
                    } else {
                        OSReport("Ioctl returned error: %d(0x%X)  sdCmd == 0x%X\n", rc, rc, req);
                    }
                } else {
                    resp[0] = isd_resp[0];
                    resp[1] = isd_resp[1];
                    resp[2] = isd_resp[2];
                    resp[3] = isd_resp[3];
                }
            }
        } else {
            OSReport("Command NULL response ptr\n");
            rc = ISD_ERR;
        }
    }

    if ((resp == NULL || cb == NULL || rc != 0) && req != NULL) {
        iosFree(isd_heap, req);
    }
    return rc;
}

// Selects the card and switches it to 512-byte blocks, 4-bit bus and the fast clock.
static inline s32 sduOptimise(ISDDevice* dev) {
    u32 resp[4];
    u32 resp2[4];
    s32 rc;

    rc = __ISDSendCommand(dev->fd, 7, 3, 2, dev->rca, NULL, 0, 0, 0, resp, NULL, NULL);
    if (rc != 0) {
        OSReport("Command returned status: %d\n", rc);
    } else {
        dev->state = 1;
        rc = __ISDSendCommand(dev->fd, 0x10, 3, 1, 0x200, NULL, 0, 0, 0, resp2, NULL, NULL);
        if (rc != 0) {
            OSReport("     %s ERROR: %d\n", __FUNCTION__, rc);
        } else {
            rc = sduDatabuswidth(dev, 4);
            if (rc != 0) {
                OSReport("     %s ERROR: %d\n", __FUNCTION__, rc);
            } else {
                rc = sduSetClock(dev->fd, 1);
                if (rc != 0) {
                    OSReport("     %s ERROR: %d\n", __FUNCTION__, rc);
                }
            }
        }
    }
    return rc;
}

s32 ISD_ResetDevice(ISDDevice* dev) {
    s32 rc;

    if (isd_heap < 0) {
        rc = ISD_ERR;
    } else {
        OSLockMutex(&isd_mutex);
        if (isd_busy != 0) {
            OSUnlockMutex(&isd_mutex);
            rc = ISD_ERR;
        } else {
            isd_busy = 1;
            OSUnlockMutex(&isd_mutex);
            rc = IOS_Ioctl(dev->fd, 4, NULL, 0, isd_reg_buf, 4);
            if (rc != 0) {
                if (rc == ISD_ERR_SDHC) {
                    OSReport("  SDHC card inserted; format not supported\n");
                } else {
                    OSReport("   +++ SD RESET Failed:  %d +++\n", rc);
                }
                dev->rca = 0;
                isd_busy = 0;
            } else {
                dev->rca = *isd_reg_buf;
                isd_card_type = ISD_CARD_MEMORY;
                dev->state = 2;
                isd_busy = 0;
            }
        }
    }

    if (rc != 0) {
        OSReport("ISD_ResetDevice exiting w/ error 0x%X\n", rc);
    }
    return rc;
}

// Stripped from ET.
s32 ISD_ResetDeviceAsync(ISDDevice* dev, ISDCallback cb, void* cb_arg) {
    ISDAsync* ctx;
    s32 rc;

    ctx = (ISDAsync*)iosAlloc(isd_heap, sizeof(ISDAsync));
    ctx->cb = cb;
    ctx->cb_arg = cb_arg;
    ctx->dev = dev;
    ctx->cmd = 4;
    ctx->resp = isd_reg_buf;
    ctx->req = NULL;
    rc = IOS_IoctlAsync(dev->fd, 4, NULL, 0, isd_reg_buf, 4, __ISDCallback, ctx);
    if (rc != 0) {
        OSReport("IOS_IoctlAsync Reset Error:(%x)\n", rc);
    }
    return rc;
}

s32 ISD_Open(u32 slot, ISDDevice** pp_dev);

void ISD_Close(ISDDevice* dev);

// Returns 0 when a memory card is in the slot.
s32 ISD_DetectCard(u32 slot) {
    ISDDevice dev;
    ISDDevice* p_dev = &dev;
    u32 status;
    s32 rc;

    rc = -0x33;
    if (isd_heap >= 0 && ISD_Open(slot, &p_dev) == 0) {
        if (ISD_GetDeviceStatus(p_dev, &status) == 0) {
            if (status & ISD_CARD_MEMORY) {
                rc = 0;
            } else {
                rc = ISD_ResetDevice(p_dev);
                if (isd_card_type == ISD_CARD_MEMORY) {
                    rc = 0;
                }
            }
        }
        ISD_Close(p_dev);
    }
    return rc;
}

s32 ISD_ReadMultiBlockAsync(ISDDevice* dev, u32 offset, void* buf, u32 size, ISDCallback cb, void* cb_arg);
s32 ISD_WriteMultiBlockAsync(ISDDevice* dev, u32 offset, void* buf, u32 size, ISDCallback cb, void* cb_arg);

s32 ISD_ReadBlock(ISDDevice* dev, u32 offset, void* buf, u32 size) {
    return ISD_ReadMultiBlockAsync(dev, offset, buf, size, NULL, NULL);
}

s32 ISD_WriteBlock(ISDDevice* dev, u32 offset, void* buf, u32 size) {
    return ISD_WriteMultiBlockAsync(dev, offset, buf, size, NULL, NULL);
}

s32 ISD_ReadMultiBlock(ISDDevice* dev, u32 offset, void* buf, u32 size) {
    return ISD_ReadMultiBlockAsync(dev, offset, buf, size, NULL, NULL);
}

s32 ISD_WriteMultiBlock(ISDDevice* dev, u32 offset, void* buf, u32 size) {
    return ISD_WriteMultiBlockAsync(dev, offset, buf, size, NULL, NULL);
}

// Stripped from ET; inlined into ISD_Open.
IOSFd sduOpenSlot(u32 slot) {
    IOSFd fd;

    fd = IOS_Open(isd_dev_name[slot], 0);
    if (fd < 0) {
        if (fd == -2) {
            OSReport("Already open %s\n", isd_dev_name[slot]);
        } else if (fd == -6) {
            OSReport("Does not exist %s\n", isd_dev_name[slot]);
        } else {
            OSReport("Could not open %s Err:%x\n", isd_dev_name[slot], fd);
        }
    }
    return fd;
}

// Stripped from ET.
s32 ISD_EraseBlocksAsync(ISDDevice* dev, u32 start, u32 end, ISDCallback cb, void* cb_arg) {
    u32 resp[4];
    s32 rc;

    if (dev->state == 2) {
        rc = sduOptimise(dev);
        if (rc != 0) {
            return rc;
        }
    }
    rc = __ISDSendCommand(dev->fd, 0x20, 3, 1, start, NULL, 0, 0, 0, resp, NULL, NULL);
    if (rc == 0) {
        rc = __ISDSendCommand(dev->fd, 0x21, 3, 1, end, NULL, 0, 0, 0, resp, NULL, NULL);
    }
    if (rc == 0) {
        rc = __ISDSendCommand(dev->fd, 0x26, 3, 1, 0, NULL, 0, 0, 0, isd_dma_resp, cb, cb_arg);
    }
    if (rc != 0) {
        OSReport("%s Error: Command returned status: %d\n", __FUNCTION__, rc);
    }
    return rc;
}

s32 ISD_ReadMultiBlockAsync(ISDDevice* dev, u32 offset, void* buf, u32 size, ISDCallback cb, void* cb_arg) {
    u32 blk_size;
    u32 blk_cnt;
    s32 rc;

    blk_size = size > 0x200 ? 0x200 : size;
    blk_cnt = size >> 9;
    if (isd_heap < 0) {
        rc = ISD_ERR;
        goto exit;
    }
    if (size != blk_size * blk_cnt) {
        OSReport("Error Length:%s(0x%X)\n", __FUNCTION__, size);
        rc = -4;
        goto exit;
    }
    if (offset != (offset & ~0x1FF)) {
        OSReport("Error Offset:%s(0x%X)\n", __FUNCTION__, offset);
        rc = -4;
        goto exit;
    }
    if (dev->max_offset != 0 && offset > dev->max_offset) {
        OSReport("Error Offset Size:%s(0x%X)\n", __FUNCTION__, offset);
        rc = -4;
        goto exit;
    }
    if (dev->state == 0) {
        OSReport("Error Uninitialised:\n", __FUNCTION__);
        rc = ISD_ERR;
        goto exit;
    }

    OSLockMutex(&isd_mutex);
    if (isd_busy != 0) {
        OSUnlockMutex(&isd_mutex);
        rc = ISD_ERR;
        goto exit;
    }
    isd_busy = 1;
    OSUnlockMutex(&isd_mutex);

    if (dev->state == 2) {
        rc = sduOptimise(dev);
        if (rc != 0) {
            isd_busy = 0;
            goto exit;
        }
    }

    DCInvalidateRange(buf, size);
    rc = __ISDSendCommandV(dev->fd, 0x12, 3, 1, offset, buf, blk_cnt, blk_size, 1, isd_dma_resp, cb, cb_arg);
    if (rc != 0) {
        OSReport("%s Error Block Read status: %d\n", __FUNCTION__, rc);
        isd_busy = 0;
    }
    if (cb == NULL) {
        isd_busy = 0;
    }
exit:
    return rc;
}

s32 ISD_WriteMultiBlockAsync(ISDDevice* dev, u32 offset, void* buf, u32 size, ISDCallback cb, void* cb_arg) {
    u32 blk_size;
    u32 blk_cnt;
    s32 rc;

    blk_size = size > 0x200 ? 0x200 : size;
    blk_cnt = size >> 9;
    if (isd_heap < 0) {
        rc = ISD_ERR;
        goto exit;
    }
    if (size != blk_size * blk_cnt) {
        OSReport("Error Length:%s(0x%X)\n", __FUNCTION__, size);
        rc = -4;
        goto exit;
    }
    if (offset != (offset & ~0x1FF)) {
        OSReport("Error Offset Align:%s(0x%X)\n", __FUNCTION__, offset);
        rc = -4;
        goto exit;
    }
    if (dev->max_offset != 0 && offset > dev->max_offset) {
        OSReport("Error Offset Size:%s(0x%X)\n", __FUNCTION__, offset);
        rc = -4;
        goto exit;
    }
    if (dev->state == 0) {
        OSReport("Error Uninitialised:\n", __FUNCTION__);
        rc = ISD_ERR;
        goto exit;
    }

    OSLockMutex(&isd_mutex);
    if (isd_busy != 0) {
        OSUnlockMutex(&isd_mutex);
        rc = ISD_ERR;
        goto exit;
    }
    isd_busy = 1;
    OSUnlockMutex(&isd_mutex);

    if (dev->state == 2) {
        rc = sduOptimise(dev);
        if (rc != 0) {
            isd_busy = 0;
            goto exit;
        }
    }

    DCFlushRange(buf, size);
    rc = __ISDSendCommand(dev->fd, 0x19, 3, 1, offset, buf, blk_cnt, blk_size, 1, isd_dma_resp, cb, cb_arg);
    if (rc != 0) {
        OSReport("%s Error Block Write status: %d\n", __FUNCTION__, rc);
        isd_busy = 0;
    }
    if (cb == NULL) {
        isd_busy = 0;
    }
exit:
    return rc;
}

s32 ISD_Open(u32 slot, ISDDevice** pp_dev) {
    s32 rc;
    IOSFd fd;

    rc = 0;
    if (isd_heap < 0) {
        rc = ISD_ERR;
    } else {
        if (slot >= 4) {
            OSReport("   +++ Open Invalid Slot Number\n");
            return -4;
        }
        if (*pp_dev == (ISDDevice*)NULL) {
            OSReport("   +++ Open Null Handle Pointer\n");
            return -4;
        }

        fd = sduOpenSlot(slot);
        (*pp_dev)->fd = fd;
        if ((*pp_dev)->fd < 0) {
            rc = -1;
            OSReport("   +++ Open Fail +++\n");
        }
        (*pp_dev)->state = 0;
        (*pp_dev)->unk18 = 0x4000;
        (*pp_dev)->max_offset = 0;
    }
    return rc;
}

void ISD_Close(ISDDevice* dev) {
    dev->state = 0;
    IOS_Close(dev->fd);
    dev->fd = -1;
}

s32 ISD_Init(void) {
    BOOL enabled;
    s32 rc;

    rc = 0;
    enabled = OSDisableInterrupts();
    if (isd_heap >= 0) {
        OSRestoreInterrupts(enabled);
        goto exit;
    }
    OSRestoreInterrupts(enabled);

    isd_busy = 0;
    OSInitMutex(&isd_mutex);
    isd_ipc_lo = IPCGetBufferLo();
    IPCGetBufferHi();
    isd_heap = iosCreateHeap(isd_ipc_lo, 0x800);
    if (isd_heap < 0) {
        rc = -0x16;
        goto exit;
    }
    isd_reg_buf = (u32*)iosAlloc(isd_heap, 0x20);
    if (!isd_reg_buf) {
        OSReport("Not enough IPC heap1\n");
        rc = -0x16;
        goto exit;
    }
    isd_resp = (u32*)iosAlloc(isd_heap, 0x20);
    if (!isd_resp) {
        OSReport("Not enough IPC heap2\n");
        rc = -0x16;
        goto exit;
    }
    isd_dma_resp = (u32*)iosAlloc(isd_heap, 0x20);
    if (!isd_dma_resp) {
        OSReport("Not enough IPC heap3\n");
        rc = -0x16;
        goto exit;
    }
    isd_hc_req = (ISDHCRequest*)iosAlloc(isd_heap, 0x20);
    if (!isd_hc_req) {
        OSReport("Not enough IPC heap4\n");
        rc = -0x16;
        goto exit;
    }
    isd_vec = (IOSIoVector*)iosAlloc(isd_heap, 0x60);
    if (!isd_vec) {
        OSReport("Not enough IPC heap6\n");
        rc = -0x16;
        goto exit;
    }

    IPCSetBufferLo((u8*)isd_ipc_lo + 0x800);
    strcpy(isd_dev_name[0], "/dev/sdio/slot0");
    strcpy(isd_dev_name[1], "/dev/sdio/slot1");
    strcpy(isd_dev_name[2], "/dev/sdio/slotw");

exit:
    return rc;
}

s32 ISD_ReadCardRegister(ISDDevice* dev, s32 reg, u32* buf, u32 size) {
    u32 resp[4];
    u32 resp_type;
    u32 rca;
    s32 rc;

    rca = dev->rca;
    resp_type = 3;
    if (isd_heap < 0) {
        rc = ISD_ERR;
        goto exit;
    }
    if (dev->state == 0) {
        rc = ISD_ERR;
        goto exit;
    }

    OSLockMutex(&isd_mutex);
    if (isd_busy != 0) {
        OSUnlockMutex(&isd_mutex);
        rc = ISD_ERR;
        goto exit;
    }
    isd_busy = 1;
    OSUnlockMutex(&isd_mutex);

    if (dev->state == 1) {
        rc = __ISDSendCommand(dev->fd, 7, 3, 2, 0, NULL, 0, 0, 0, resp, NULL, NULL);
        dev->state = 2;
        if (rc == ISD_ERR_TIMEOUT) {
            rc = 0;
        } else if (rc != 0) {
            OSReport("   sduDeSelect:  got %X, expected timeout (%X)\n", rc, ISD_ERR_TIMEOUT);
        }
        isd_busy = 0;
        if (rc != 0) {
            goto exit;
        }
    }

    if (buf == NULL || size == 0) {
        isd_busy = 0;
        rc = ISD_ERR;
        goto exit;
    }

    switch (reg) {
    case 9:
    case 10:
        resp_type = 3;
        break;
    case 0x33:
        rc = sduGetSCR(dev, buf);
        isd_busy = 0;
        goto exit;
    case 0x29:
        rc = 0;
        if (buf != NULL) {
            rc = IOS_Ioctl(dev->fd, 0xC, NULL, 0, isd_reg_buf, 4);
            if (rc != 0) {
                OSReport("Get OCR Failed, rc = %d\n", rc);
            } else {
                *buf = *isd_reg_buf;
            }
        }
        isd_busy = 0;
        goto exit;
    case 3:
        *buf = dev->rca;
        isd_busy = rc = 0;
        goto exit;
    }

    rc = __ISDSendCommand(dev->fd, reg, 3, resp_type, rca, NULL, 0, 0, 0, buf, NULL, NULL);
    isd_busy = 0;

exit:
    if (rc != 0) {
        OSReport("%s Error: Command returned status: %d\n", __FUNCTION__, rc);
    }
    return rc;
}


s32 sduDatabuswidth(ISDDevice* dev, u32 width) {
    u32 resp[4];
    u32 reg;
    s32 rc;
    u32 bus;

    bus = width == 4 ? 2 : 0;
    rc = __ISDSendCommand(dev->fd, 0x37, 3, 1, dev->rca, NULL, 0, 0, 0, resp, NULL, NULL);
    if (rc != 0) {
        OSReport("SD_REG_ACMD");
        OSReport("%s: Bad ioctl 0x%x\n", __FUNCTION__, rc);
        return rc;
    }
    rc = __ISDSendCommand(dev->fd, 6, 3, 1, bus, NULL, 0, 0, 0, resp, NULL, NULL);
    if (rc != 0) {
        OSReport("SET_BUS_WIDTH");
        OSReport("%s: Bad ioctl 0x%x\n", __FUNCTION__, rc);
        return rc;
    }
    rc = ISD_GetHCRegister(dev, 0x28, &reg, 1);
    if (rc != 0) {
        OSReport("%s: Error ISD_GetHCRegister %d\n", __FUNCTION__, 0x28);
        return rc;
    }
    if (bus != 0) {
        reg |= 2;
    } else {
        reg &= ~2;
    }
    rc = ISD_SetHCRegister(dev, 0x28, &reg, 1);
    if (rc != 0) {
        OSReport("%s: Error ISD_SetHCRegister %d\n", __FUNCTION__, 0x28);
        return rc;
    }
    return rc;
}

#pragma push
#pragma dont_inline on
s32 sduGetSCR(ISDDevice* dev, u32* buf) {
    u32 resp[4];
    s32 rc;

    rc = ISD_ERR;
    if (buf != NULL) {
        OSReport("   *** Get SCR ***\n");
        __ISDSendCommand(dev->fd, 0x10, 3, 1, 4, NULL, 0, 0, 0, resp, NULL, NULL);
        rc = __ISDSendCommand(dev->fd, 0x37, 3, 1, dev->rca, NULL, 0, 0, 0, resp, NULL, NULL);
        if (rc != 0) {
            OSReport("SD_REG_ACMD Error, status = %d\n", rc);
            return rc;
        }
        rc = __ISDSendCommand(dev->fd, 0x33, 3, 1, 0, buf, 1, 4, 0, resp, NULL, NULL);
        if (rc != 0) {
            OSReport("Get SCR Failed, status = %d\n", rc);
        } else {
            OSReport("   *** SCR = %08X %08X ***\n", buf[0], buf[1]);
        }
        rc = __ISDSendCommand(dev->fd, 0x10, 3, 1, 0x200, NULL, 0, 0, 0, resp, NULL, NULL);
    }
    return rc;
}
#pragma pop

// Stripped from ET.
s32 ISD_GetCardSize(ISDDevice* dev, u32* p_size) {
    u32 csd[4];
    s32 rc;

    rc = ISD_ReadCardRegister(dev, 9, csd, sizeof(csd));
    if (rc != 0) {
        OSReport("GetCardSize error status: %d\n", rc);
        return rc;
    }
    *p_size = csd[1];
    return rc;
}

s32 ISD_RegisterDeviceIntrHandler(ISDDevice* dev, ISDCallback cb, u32* p_event) {
    u32 event;
    s32 rc;

    if (isd_heap < 0) {
        rc = ISD_ERR;
    } else if (cb == NULL) {
        rc = ISD_ERR;
    } else {
        if (p_event != NULL) {
            event = *p_event;
        } else {
            rc = -4;
            goto exit;
        }
        rc = __ISDSendCommand(dev->fd, 0x40, 0, 0, event, NULL, 0, 0, 0, isd_dma_resp, cb, p_event);
    }

exit:
    if (rc != 0) {
        OSReport("%s Error: %d\n", __FUNCTION__, rc);
    }
    return rc;
}

// Stripped from ET.
s32 ISD_UnregisterDeviceIntrHandler(ISDDevice* dev) {
    s32 rc;

    rc = IOS_Ioctl(dev->fd, 0x41, NULL, 0, NULL, 0);
    if (rc != 0) {
        OSReport("%s Error: %d\n", __FUNCTION__, rc);
    }
    return rc;
}
