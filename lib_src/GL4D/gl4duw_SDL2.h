/*!\file gl4duw_SDL2.h
 *
 * \brief Fonctions haut-niveau de GL4Dummies pour la gestion des fenêtres avec SDL2
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date Februry 01 2016
 */

#ifndef _GL4DUW_SDL2_H
#define _GL4DUW_SDL2_H

#include "gl4du.h"
#include <SDL.h>
#include <SDL_opengl.h>

#define GL4DK_UNKNOWN SDLK_UNKNOWN
#define GL4DK_RETURN SDLK_RETURN
#define GL4DK_ESCAPE SDLK_ESCAPE
#define GL4DK_BACKSPACE SDLK_BACKSPACE
#define GL4DK_TAB SDLK_TAB
#define GL4DK_SPACE SDLK_SPACE
#define GL4DK_EXCLAIM SDLK_EXCLAIM
#define GL4DK_QUOTEDBL SDLK_QUOTEDBL
#define GL4DK_HASH SDLK_HASH
#define GL4DK_PERCENT SDLK_PERCENT
#define GL4DK_DOLLAR SDLK_DOLLAR
#define GL4DK_AMPERSAND SDLK_AMPERSAND
#define GL4DK_QUOTE SDLK_QUOTE
#define GL4DK_LEFTPAREN SDLK_LEFTPAREN
#define GL4DK_RIGHTPAREN SDLK_RIGHTPAREN
#define GL4DK_ASTERISK SDLK_ASTERISK
#define GL4DK_PLUS SDLK_PLUS
#define GL4DK_COMMA SDLK_COMMA
#define GL4DK_MINUS SDLK_MINUS
#define GL4DK_PERIOD SDLK_PERIOD
#define GL4DK_SLASH SDLK_SLASH
#define GL4DK_0 SDLK_0
#define GL4DK_1 SDLK_1
#define GL4DK_2 SDLK_2
#define GL4DK_3 SDLK_3
#define GL4DK_4 SDLK_4
#define GL4DK_5 SDLK_5
#define GL4DK_6 SDLK_6
#define GL4DK_7 SDLK_7
#define GL4DK_8 SDLK_8
#define GL4DK_9 SDLK_9
#define GL4DK_COLON SDLK_COLON
#define GL4DK_SEMICOLON SDLK_SEMICOLON
#define GL4DK_LESS SDLK_LESS
#define GL4DK_EQUALS SDLK_EQUALS
#define GL4DK_GREATER SDLK_GREATER
#define GL4DK_QUESTION SDLK_QUESTION
#define GL4DK_AT SDLK_AT
#define GL4DK_LEFTBRACKET SDLK_LEFTBRACKET
#define GL4DK_BACKSLASH SDLK_BACKSLASH
#define GL4DK_RIGHTBRACKET SDLK_RIGHTBRACKET
#define GL4DK_CARET SDLK_CARET
#define GL4DK_UNDERSCORE SDLK_UNDERSCORE
#define GL4DK_BACKQUOTE SDLK_BACKQUOTE
#define GL4DK_a SDLK_a
#define GL4DK_b SDLK_b
#define GL4DK_c SDLK_c
#define GL4DK_d SDLK_d
#define GL4DK_e SDLK_e
#define GL4DK_f SDLK_f
#define GL4DK_g SDLK_g
#define GL4DK_h SDLK_h
#define GL4DK_i SDLK_i
#define GL4DK_j SDLK_j
#define GL4DK_k SDLK_k
#define GL4DK_l SDLK_l
#define GL4DK_m SDLK_m
#define GL4DK_n SDLK_n
#define GL4DK_o SDLK_o
#define GL4DK_p SDLK_p
#define GL4DK_q SDLK_q
#define GL4DK_r SDLK_r
#define GL4DK_s SDLK_s
#define GL4DK_t SDLK_t
#define GL4DK_u SDLK_u
#define GL4DK_v SDLK_v
#define GL4DK_w SDLK_w
#define GL4DK_x SDLK_x
#define GL4DK_y SDLK_y
#define GL4DK_z SDLK_z
#define GL4DK_CAPSLOCK SDLK_CAPSLOCK
#define GL4DK_F1 SDLK_F1
#define GL4DK_F2 SDLK_F2
#define GL4DK_F3 SDLK_F3
#define GL4DK_F4 SDLK_F4
#define GL4DK_F5 SDLK_F5
#define GL4DK_F6 SDLK_F6
#define GL4DK_F7 SDLK_F7
#define GL4DK_F8 SDLK_F8
#define GL4DK_F9 SDLK_F9
#define GL4DK_F10 SDLK_F10
#define GL4DK_F11 SDLK_F11
#define GL4DK_F12 SDLK_F12
#define GL4DK_PRINTSCREEN SDLK_PRINTSCREEN
#define GL4DK_SCROLLLOCK SDLK_SCROLLLOCK
#define GL4DK_PAUSE SDLK_PAUSE
#define GL4DK_INSERT SDLK_INSERT
#define GL4DK_HOME SDLK_HOME
#define GL4DK_PAGEUP SDLK_PAGEUP
#define GL4DK_DELETE SDLK_DELETE
#define GL4DK_END SDLK_END
#define GL4DK_PAGEDOWN SDLK_PAGEDOWN
#define GL4DK_RIGHT SDLK_RIGHT
#define GL4DK_LEFT SDLK_LEFT
#define GL4DK_DOWN SDLK_DOWN
#define GL4DK_UP SDLK_UP
#define GL4DK_NUMLOCKCLEAR SDLK_NUMLOCKCLEAR
#define GL4DK_KP_DIVIDE SDLK_KP_DIVIDE
#define GL4DK_KP_MULTIPLY SDLK_KP_MULTIPLY
#define GL4DK_KP_MINUS SDLK_KP_MINUS
#define GL4DK_KP_PLUS SDLK_KP_PLUS
#define GL4DK_KP_ENTER SDLK_KP_ENTER
#define GL4DK_KP_1 SDLK_KP_1
#define GL4DK_KP_2 SDLK_KP_2
#define GL4DK_KP_3 SDLK_KP_3
#define GL4DK_KP_4 SDLK_KP_4
#define GL4DK_KP_5 SDLK_KP_5
#define GL4DK_KP_6 SDLK_KP_6
#define GL4DK_KP_7 SDLK_KP_7
#define GL4DK_KP_8 SDLK_KP_8
#define GL4DK_KP_9 SDLK_KP_9
#define GL4DK_KP_0 SDLK_KP_0
#define GL4DK_KP_PERIOD SDLK_KP_PERIOD
#define GL4DK_APPLICATION SDLK_APPLICATION
#define GL4DK_POWER SDLK_POWER
#define GL4DK_KP_EQUALS SDLK_KP_EQUALS
#define GL4DK_F13 SDLK_F13
#define GL4DK_F14 SDLK_F14
#define GL4DK_F15 SDLK_F15
#define GL4DK_F16 SDLK_F16
#define GL4DK_F17 SDLK_F17
#define GL4DK_F18 SDLK_F18
#define GL4DK_F19 SDLK_F19
#define GL4DK_F20 SDLK_F20
#define GL4DK_F21 SDLK_F21
#define GL4DK_F22 SDLK_F22
#define GL4DK_F23 SDLK_F23
#define GL4DK_F24 SDLK_F24
#define GL4DK_EXECUTE SDLK_EXECUTE
#define GL4DK_HELP SDLK_HELP
#define GL4DK_MENU SDLK_MENU
#define GL4DK_SELECT SDLK_SELECT
#define GL4DK_STOP SDLK_STOP
#define GL4DK_AGAIN SDLK_AGAIN
#define GL4DK_UNDO SDLK_UNDO
#define GL4DK_CUT SDLK_CUT
#define GL4DK_COPY SDLK_COPY
#define GL4DK_PASTE SDLK_PASTE
#define GL4DK_FIND SDLK_FIND
#define GL4DK_MUTE SDLK_MUTE
#define GL4DK_VOLUMEUP SDLK_VOLUMEUP
#define GL4DK_VOLUMEDOWN SDLK_VOLUMEDOWN
#define GL4DK_KP_COMMA SDLK_KP_COMMA
#define GL4DK_KP_EQUALSAS400 SDLK_KP_EQUALSAS400
#define GL4DK_ALTERASE SDLK_ALTERASE
#define GL4DK_SYSREQ SDLK_SYSREQ
#define GL4DK_CANCEL SDLK_CANCEL
#define GL4DK_CLEAR SDLK_CLEAR
#define GL4DK_PRIOR SDLK_PRIOR
#define GL4DK_RETURN2 SDLK_RETURN2
#define GL4DK_SEPARATOR SDLK_SEPARATOR
#define GL4DK_OUT SDLK_OUT
#define GL4DK_OPER SDLK_OPER
#define GL4DK_CLEARAGAIN SDLK_CLEARAGAIN
#define GL4DK_CRSEL SDLK_CRSEL
#define GL4DK_EXSEL SDLK_EXSEL
#define GL4DK_KP_00 SDLK_KP_00
#define GL4DK_KP_000 SDLK_KP_000
#define GL4DK_THOUSANDSSEPARATOR SDLK_THOUSANDSSEPARATOR
#define GL4DK_DECIMALSEPARATOR SDLK_DECIMALSEPARATOR
#define GL4DK_CURRENCYUNIT SDLK_CURRENCYUNIT
#define GL4DK_CURRENCYSUBUNIT SDLK_CURRENCYSUBUNIT
#define GL4DK_KP_LEFTPAREN SDLK_KP_LEFTPAREN
#define GL4DK_KP_RIGHTPAREN SDLK_KP_RIGHTPAREN
#define GL4DK_KP_LEFTBRACE SDLK_KP_LEFTBRACE
#define GL4DK_KP_RIGHTBRACE SDLK_KP_RIGHTBRACE
#define GL4DK_KP_TAB SDLK_KP_TAB
#define GL4DK_KP_BACKSPACE SDLK_KP_BACKSPACE
#define GL4DK_KP_A SDLK_KP_A
#define GL4DK_KP_B SDLK_KP_B
#define GL4DK_KP_C SDLK_KP_C
#define GL4DK_KP_D SDLK_KP_D
#define GL4DK_KP_E SDLK_KP_E
#define GL4DK_KP_F SDLK_KP_F
#define GL4DK_KP_XOR SDLK_KP_XOR
#define GL4DK_KP_POWER SDLK_KP_POWER
#define GL4DK_KP_PERCENT SDLK_KP_PERCENT
#define GL4DK_KP_LESS SDLK_KP_LESS
#define GL4DK_KP_GREATER SDLK_KP_GREATER
#define GL4DK_KP_AMPERSAND SDLK_KP_AMPERSAND
#define GL4DK_KP_DBLAMPERSAND SDLK_KP_DBLAMPERSAND
#define GL4DK_KP_VERTICALBAR SDLK_KP_VERTICALBAR
#define GL4DK_KP_DBLVERTICALBAR SDLK_KP_DBLVERTICALBAR
#define GL4DK_KP_COLON SDLK_KP_COLON
#define GL4DK_KP_HASH SDLK_KP_HASH
#define GL4DK_KP_SPACE SDLK_KP_SPACE
#define GL4DK_KP_AT SDLK_KP_AT
#define GL4DK_KP_EXCLAM SDLK_KP_EXCLAM
#define GL4DK_KP_MEMSTORE SDLK_KP_MEMSTORE
#define GL4DK_KP_MEMRECALL SDLK_KP_MEMRECALL
#define GL4DK_KP_MEMCLEAR SDLK_KP_MEMCLEAR
#define GL4DK_KP_MEMADD SDLK_KP_MEMADD
#define GL4DK_KP_MEMSUBTRACT SDLK_KP_MEMSUBTRACT
#define GL4DK_KP_MEMMULTIPLY SDLK_KP_MEMMULTIPLY
#define GL4DK_KP_MEMDIVIDE SDLK_KP_MEMDIVIDE
#define GL4DK_KP_PLUSMINUS SDLK_KP_PLUSMINUS
#define GL4DK_KP_CLEAR SDLK_KP_CLEAR
#define GL4DK_KP_CLEARENTRY SDLK_KP_CLEARENTRY
#define GL4DK_KP_BINARY SDLK_KP_BINARY
#define GL4DK_KP_OCTAL SDLK_KP_OCTAL
#define GL4DK_KP_DECIMAL SDLK_KP_DECIMAL
#define GL4DK_KP_HEXADECIMAL SDLK_KP_HEXADECIMAL
#define GL4DK_LCTRL SDLK_LCTRL
#define GL4DK_LSHIFT SDLK_LSHIFT
#define GL4DK_LALT SDLK_LALT
#define GL4DK_LGUI SDLK_LGUI
#define GL4DK_RCTRL SDLK_RCTRL
#define GL4DK_RSHIFT SDLK_RSHIFT
#define GL4DK_RALT SDLK_RALT
#define GL4DK_RGUI SDLK_RGUI
#define GL4DK_MODE SDLK_MODE
#define GL4DK_AUDIONEXT SDLK_AUDIONEXT
#define GL4DK_AUDIOPREV SDLK_AUDIOPREV
#define GL4DK_AUDIOSTOP SDLK_AUDIOSTOP
#define GL4DK_AUDIOPLAY SDLK_AUDIOPLAY
#define GL4DK_AUDIOMUTE SDLK_AUDIOMUTE
#define GL4DK_MEDIASELECT SDLK_MEDIASELECT
#define GL4DK_WWW SDLK_WWW
#define GL4DK_MAIL SDLK_MAIL
#define GL4DK_CALCULATOR SDLK_CALCULATOR
#define GL4DK_COMPUTER SDLK_COMPUTER
#define GL4DK_AC_SEARCH SDLK_AC_SEARCH
#define GL4DK_AC_HOME SDLK_AC_HOME
#define GL4DK_AC_BACK SDLK_AC_BACK
#define GL4DK_AC_FORWARD SDLK_AC_FORWARD
#define GL4DK_AC_STOP SDLK_AC_STOP
#define GL4DK_AC_REFRESH SDLK_AC_REFRESH
#define GL4DK_AC_BOOKMARKS SDLK_AC_BOOKMARKS
#define GL4DK_BRIGHTNESSDOWN SDLK_BRIGHTNESSDOWN
#define GL4DK_BRIGHTNESSUP SDLK_BRIGHTNESSUP
#define GL4DK_DISPLAYSWITCH SDLK_DISPLAYSWITCH
#define GL4DK_KBDILLUMTOGGLE SDLK_KBDILLUMTOGGLE
#define GL4DK_KBDILLUMDOWN SDLK_KBDILLUMDOWN
#define GL4DK_KBDILLUMUP SDLK_KBDILLUMUP
#define GL4DK_EJECT SDLK_EJECT
#define GL4DK_SLEEP SDLK_SLEEP

/* window flags */
/*!\brief fullscreen window */
#define GL4DW_FULLSCREEN SDL_WINDOW_FULLSCREEN
/*!\brief window usable with OpenGL context */
#define GL4DW_OPENGL SDL_WINDOW_OPENGL
/*!\brief window is visible */
#define GL4DW_SHOWN SDL_WINDOW_SHOWN
/*!\brief window is not visible */
#define GL4DW_HIDDEN SDL_WINDOW_HIDDEN
/*!\brief no window decoration */
#define GL4DW_BORDERLESS SDL_WINDOW_BORDERLESS
/*!\brief window can be resized */
#define GL4DW_RESIZABLE SDL_WINDOW_RESIZABLE
/*!\brief window is minimized */
#define GL4DW_MINIMIZED SDL_WINDOW_MINIMIZED
/*!\brief window is maximized */
#define GL4DW_MAXIMIZED SDL_WINDOW_MAXIMIZED
/*!\brief window has grabbed input focus */
#define GL4DW_INPUT_GRABBED SDL_WINDOW_INPUT_GRABBED
/*!\brief window has input focus */
#define GL4DW_INPUT_FOCUS SDL_WINDOW_INPUT_FOCUS
/*!\brief window has mouse focus */
#define GL4DW_MOUSE_FOCUS SDL_WINDOW_MOUSE_FOCUS
/*!\brief fullscreen window in desktop mode (same resolution) */
#define GL4DW_FULLSCREEN_DESKTOP SDL_WINDOW_FULLSCREEN_DESKTOP
/*!\brief window not created by SDL */
#define GL4DW_FOREIGN SDL_WINDOW_FOREIGN
/*!\brief window should be created in high-DPI mode if supported */
#define GL4DW_ALLOW_HIGHDPI SDL_WINDOW_ALLOW_HIGHDPI
/* window coordinates */
/*!\brief Used to indicate that you don't care what the window position is.*/
#define GL4DW_POS_UNDEFINED SDL_WINDOWPOS_UNDEFINED
/*!\brief Used to indicate that the window position should be centered.*/
#define GL4DW_POS_CENTERED SDL_WINDOWPOS_CENTERED
/* OpenGL Context profiles */
#define GL4DW_CONTEXT_PROFILE_CORE SDL_GL_CONTEXT_PROFILE_CORE
#define GL4DW_CONTEXT_PROFILE_COMPATIBILITY SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
#define GL4DW_CONTEXT_PROFILE_ES SDL_GL_CONTEXT_PROFILE_ES
/* Mouse button states */
#define GL4D_BUTTON_LEFT SDL_BUTTON_LEFT
#define GL4D_BUTTON_MIDDLE SDL_BUTTON_MIDDLE
#define GL4D_BUTTON_RIGHT SDL_BUTTON_RIGHT
#define GL4D_BUTTON_X1 SDL_BUTTON_X1
#define GL4D_BUTTON_X2 SDL_BUTTON_X2


#ifdef __cplusplus
extern "C" {
#endif
  
  /*!\brief modifie les paramètre par défaut du contexte OpenGL.
   *
   * Cette fonction est à appeler avant \ref gl4duwCreateWindow si vous
   * souhaitez que ses paramètres soient pris en compte. Si vous
   * souhaitez modifier plus de paramètres voir la fonction
   * SDL_GL_SetAttribute.
   *
   * \param glMajorVersion version majeure d'OpenGL, par défaut vaut 3.
   * \param glMinorVersion version mineure d'OpenGL, par défaut vaut 2.
   * \param glProfileMask modifie le profile d'OpenGL, par défaut vaut
   * GL4DW_CONTEXT_PROFILE_CORE mais peut aussi prendre
   * GL4DW_CONTEXT_PROFILE_COMPATIBILITY, GL4DW_CONTEXT_PROFILE_ES.
   * \param glDoubleBuffer modifie l'état actif ou non du double buffer,
   * par défaut vaut 1 (vrai).
   * \param glDepthSize modifie la dimension (nombre de bits utilisés)
   * du buffer de profondeur, par défaut vaut 16.
   * \see gl4duwCreateWindow
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwSetGLAttributes(int glMajorVersion, int glMinorVersion, int glProfileMask, int glDoubleBuffer, int glDepthSize);
  /*!\brief créé une fenêtre SDL avec un contexte OpenGL.
   *
   * Pour modifier les valeurs utilisée par le contexte OpenGL, utiliser
   * la fonction \ref gl4duwSetGLAttributes.
   *
   * \param argc nombre d'arguments passés au programme (premier argument de la fonction main).
   * \param argv liste des arguments passés au programme (second argument de la fonction main).
   * \param title titre de la fenêtre SDL à créer.
   * \param x la postion x de la fenêtre à créer ou GL4DW_POS_CENTERED, ou GL4DW_POS_UNDEFINED.
   * \param y la postion y de la fenêtre à créer ou GL4DW_POS_CENTERED, ou GL4DW_POS_UNDEFINED.
   * \param width la largeur de la fenêtre à créer.
   * \param height la hauteur de la fenêtre à créer.
   * \param wflags des options sur la fenêtre à créer. Peut être 0 ou
   * une ou toute combinaison (via OR "|") de : GL4DW_FULLSCREEN,
   * GL4DW_FULLSCREEN_DESKTOP, GL4DW_OPENGL,
   * GL4DW_HIDDEN, GL4DW_BORDERLESS, GL4DW_RESIZABLE,
   * GL4DW_MINIMIZED, GL4DW_MAXIMIZED,
   * GL4DW_INPUT_GRABBED, GL4DW_ALLOW_HIGHDPI.
   * \return GL_TRUE en cas de réussite, GL_FALSE en cas d'échec.
   * \see gl4duwSetGLAttributes
   * \see SDL_CreateWindow
   */
  GL4DAPI GLboolean  GL4DAPIENTRY gl4duwCreateWindow(int argc, char ** argv, const char * title, int x, int y, int width, int height, Uint32 wflags);
  /*!\brief recherche et positionne "courant" une fenêtre en fonction de son titre.
   *
   * La fenêtre courante est celle qui est affectée par les appelles des
   * fonctions telles que \ref gl4duwResizeFunc, \ref gl4duwKeyDownFunc,
   * \ref gl4duwKeyUpFunc, \ref gl4duwIdleFunc, \ref gl4duwDisplayFunc.
   *
   * \param title titre de la fenêtre SDL recherchée.
   * \return GL_TRUE en cas de réussite, GL_FALSE en cas d'échec.
   */
  GL4DAPI GLboolean  GL4DAPIENTRY gl4duwBindWindow(const char * title);
  /*!\brief récupère la largeur et la hauteur de la fenêtre courante.
   * \param w pointeur vers entier où sera renseignée la largeur de la fenêtre.
   * \param h pointeur vers entier où sera renseignée la hauteur de la fenêtre.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwGetWindowSize(int * w, int * h);
  /*!\brief boucle principale événement/simulation/affichage */
  GL4DAPI void       GL4DAPIENTRY gl4duwMainLoop(void);
  /*!\brief affecte la fonction appelée lors du resize.
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * resize. Cette dernière reçoit en paramètre la largeur \a width et
   * la hauteur \a height de la fenêtre.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwResizeFunc(void (*func)(int width, int height));
  /*!\brief affecte la fonction appelée lors de l'événement key down.
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * de l'événement keydown. Cette dernière reçoit en paramètre le
   * code \a keycode de la touche enfoncée (voir les macros
   * GL4DK_xxx).
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwKeyDownFunc(void (*func)(int keycode));
  /*!\brief affecte la fonction appelée lors de l'événement key up.
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * de l'événement keyup. Cette dernière reçoit en paramètre le
   * code \a keycode de la touche relachée (voir les macros
   * GL4DK_xxx).
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwKeyUpFunc(void (*func)(int keycode));
  /*!\brief affecte la fonction appelée lorsqu'un utilisateur appuie
   * ou relache un bouton de la souris dans la fenêtre.
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * de l'événement lié à la souris. Cette dernière reçoit en
   * paramètre : le code \a button du bouton concerné (voir les macros
   * GL4D_MOUSE_xxx), l'état \a state du bouton, l'abscisse \a x de la
   * souris et son ordonée \a y.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwMouseFunc(void (*func)(int button, int state, int x, int y));
  /*!\brief affecte la fonction appelée lorsqu'un utilisateur déplace
   * la souris dans la fenêtre tout en ayant un ou plusieurs boutons
   * enfoncés. 
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * déplacement "drag" de la souris. Cette dernière reçoit en
   * paramètre : l'abscisse \a x de la souris et son ordonée \a y.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwMotionFunc(void (*func)(int x, int y));
  /*!\brief affecte la fonction appelée lorsqu'un utilisateur déplace
   * la souris dans la fenêtre sans qu'aucun bouton ne soit enfoncé
   * \param func pointeur vers fonction utilisateur à appeler lors du
   * "drop" de la souris. Cette dernière reçoit en paramètre :
   * l'abscisse \a x de la souris et son ordonée \a y.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwPassiveMotionFunc(void (*func)(int x, int y));
  /*!\brief affecte la fonction appelée lors de l'idle
   * (calcul/simulation avant affichage).
   * \param func pointeur vers fonction utilisateur à appeler pour
   * réaliser la phase calcul/simulation.
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwIdleFunc(void (*func)(void));
  /*!\brief affecte la fonction appelée lors de l'affichage. 
   * \param func pointeur vers fonction utilisateur à appeler pour
   * réaliser le display (dessin).
   */
  GL4DAPI void       GL4DAPIENTRY gl4duwDisplayFunc(void (*func)(void));
  
#ifdef __cplusplus
}
#endif

#endif
