/******************************************************************************
 * @file <Filename>.h
 *
 * @author <Author>
 * @date   <Date>
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************
 *
 * @brief <Some short descrition>
 *
 * @details <A more detailed description>
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "AppContext.h"

#include "stddef.h"
/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/
static ApplicationContext_t *gAppData = NULL;

void AppContext_Set(ApplicationContext_t* ctx)
{
	gAppData = ctx;
}

ApplicationContext_t* AppContext_Get(void)
{
    return gAppData;
}
/***** PRIVATE FUNCTIONS *****************************************************/
