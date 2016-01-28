#include "../stdafx.h"
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{

#define Lock(ppobj, pobj) (*ppobj = pobj)
#define Unlock(ppobj) (*ppobj = NULL)

		/*VOID LinkWindow(
			PVIS pwnd,
			PVIS pwndInsert,
			PVIS *ppwndFirst)
		{
			if (*ppwndFirst == pwnd)
			{
				return;
			}
			if (pwndInsert == PVIS_TOP)
			{
			LinkTop:
				Lock(&pwnd->spvisNext, *ppwndFirst);
				Lock(ppwndFirst, pwnd);
			}
			else
			{
				if (pwndInsert == PVIS_BOTTOM)
				{
					if (((pwndInsert = *ppwndFirst) == NULL) || TestWF(pwndInsert, WFBOTTOMMOST))
						goto LinkTop;

					while (pwndInsert->spvisNext != NULL)
					{
						if (TestWF(pwndInsert->spvisNext, WFBOTTOMMOST))
						{
							break;
						}
						pwndInsert = pwndInsert->spvisNext;
					}
				}
				ASSERT(pwnd != pwndInsert);
				ASSERT(pwnd != pwndInsert->spvisNext);
				ASSERT(!TestWF(pwndInsert, WFDESTROYED));
				ASSERT(pwnd->spvisParent == pwndInsert->spvisParent);
				Lock(&pwnd->spvisNext, pwndInsert->spvisNext);
				Lock(&pwndInsert->spvisNext, pwnd);
			}
		}

		VOID UnlinkWindow(
			PVIS pwndUnlink,
			PVIS *ppwndFirst)
		{
			PVIS pwnd;
			pwnd = *ppwndFirst;

			if (pwnd == pwndUnlink)
				goto Unlock;

			while (pwnd != NULL)
			{
				if (pwnd->spvisNext == pwndUnlink)
				{
					ppwndFirst = &pwnd->spvisNext;
				Unlock:
					Lock(ppwndFirst, pwndUnlink->spvisNext);
					Unlock(&pwndUnlink->spvisNext);
					return;
				}

				pwnd = pwnd->spvisNext;
			}
			return;
		}*/
	}
}
