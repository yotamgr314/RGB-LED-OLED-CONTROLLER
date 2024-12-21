#ifndef OLEDC_SHAPE_HANDLER_H
#define	OLEDC_SHAPE_HANDLER_H

#include <stdint.h>
#include "oledC_shapes.h"

void oledC_redrawAll(void);
void oledC_redrawTo(uint8_t endInd);
void oledC_redrawSome(uint8_t startInd, uint8_t endInd);
void oledC_redrawFrom(uint8_t startInd);
void oledC_redrawIndex(uint8_t indShape);
void oledC_addShape(uint8_t drawIndex, enum OLEDC_SHAPE shape_type, shape_params_t *params);
void oledC_removeShape(uint8_t drawIndex);
void oledC_eraseShape(uint8_t indShape, uint16_t eraseColor);
void oledC_eraseAll(uint16_t eraseColor);
shape_t* oledC_getShape(uint8_t index);

#endif	/* OLEDC_SHAPE_HANDLER_H */

