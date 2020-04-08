/**
 * @file hal_config.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of configuration operations for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_config.h"

#include <string.h>
#include <sys/mman.h>

#include "hal_base.h"
#include "hal_io.h"  // For hal_wait_for_ready

/**
 * @brief Write configuration to hardware registers
 * @param ctx Pointer to HAL context
 * @param config Pointer to configuration data
 * @return true if successful, false on error
 */
static bool write_config(hal_context_t* ctx, const void* config) {
  if (!ctx || !config) {
    return false;
  }

  /* Wait for hardware to be ready */
  if (!hal_wait_for_ready(ctx)) {
    return false;
  }

  /* Get the mapped register structure */
  hal_controller_ir_t* ir = (hal_controller_ir_t*)ctx->mapped_memory;
  if (!ir) {
    return false;
  }

  /* Copy configuration data to registers */
  memcpy(ir, config, sizeof(hal_controller_ir_t));

  return true;
}

/**
 * @brief Configure the LSU unit
 * @param ctx Pointer to HAL context
 * @param config Pointer to LSU configuration
 * @return true if successful, false on error
 */
bool hal_configure_lsu(hal_context_t* ctx, const hal_lsu_config_t* config) {
  hal_controller_ir_t ir = {0};
  memcpy(&ir.ir_data.lsu, config, sizeof(hal_lsu_config_t));
  return write_config(ctx, &ir);
}

/**
 * @brief Configure the systolic array
 * @param ctx Pointer to HAL context
 * @param config Pointer to systolic array configuration
 * @return true if successful, false on error
 */
bool hal_configure_systolic(hal_context_t* ctx,
                            const hal_systolic_config_t* config) {
  hal_controller_ir_t ir = {0};
  memcpy(&ir.ir_data.systolic_array, config, sizeof(hal_systolic_config_t));
  return write_config(ctx, &ir);
}

/**
 * @brief Configure the img2col unit
 * @param ctx Pointer to HAL context
 * @param config Pointer to img2col configuration
 * @return true if successful, false on error
 */
bool hal_configure_img2col(hal_context_t* ctx,
                           const hal_img2col_config_t* config) {
  hal_controller_ir_t ir = {0};
  memcpy(&ir.ir_data.img2col, config, sizeof(hal_img2col_config_t));
  return write_config(ctx, &ir);
}
