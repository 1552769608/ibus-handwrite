/*
 * UI_gtk.c -- provide UI via gtk
 *
 *  Created on: 2010-2-4
 *      Author: cai
 */


#include <clutter/clutter.h>

#include "engine.h"
#include "UI.h"

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)


static void widget_realize(ClutterActor *widget, gpointer user_data);

static gboolean paint_lines_gl(ClutterCanvas *canvas, cairo_t *cr,
		gint width, gint height, IBusHandwriteEngine * engine)
{

	printf("%s called with canvas=%p\n",__func__,canvas);

	LineStroke cl;
	int i,j;

	glClearColor(240,240,230,1);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor3us(engine->color->red,engine->color->green,engine->color->blue);

	//已经录入的笔画
	for (i = 0; i < engine->engine->strokes->len ; i++ )
	{
		printf("drawing %d th line, total %d\n",i,engine->engine->strokes->len);
		cl =  g_array_index(engine->engine->strokes,LineStroke,i);

		glBegin(GL_LINE_STRIP);

		for( j = 0 ; j < cl.segments ; ++j)
		{
			glVertex2f((float)cl.points[j].x *2 /width - 1 ,1 - (float)cl.points[j].y*2/height);
		}
		glEnd();
	}
	//当下笔画
	if ( engine->currentstroke.segments && engine->currentstroke.points )
	{
		glBegin(GL_LINE_STRIP);

		for( j = 0 ; j < engine->currentstroke.segments ; ++j)
		{
			glVertex2f((float)engine->currentstroke.points[j].x*2/width - 1,1 - (float)engine->currentstroke.points[j].y*2/height);
		}
		glEnd();
	}

	glFinish();
}

static gboolean widget_resize(GtkWidget *widget, GdkEventConfigure *event,IBusHandwriteEngine * engine)
{
#if 0
	GdkGLDrawable * gldrawable;
	GdkGLContext  * glcontext;

	gldrawable = gtk_widget_get_gl_drawable(widget);
	glcontext  = gtk_widget_get_gl_context(widget);

	g_assert(gdk_gl_drawable_gl_begin(gldrawable,glcontext));

	glViewport(0,0,event->width,event->height);

	glFinish();

	gdk_gl_drawable_gl_end(gldrawable);

	return TRUE;
#endif
}

static void glwidget_realize(GtkWidget *widget, gpointer user_data)
{
#if 0
	GdkGLDrawable * gldrawable;
	GdkGLContext  * glcontext;

	gldrawable = gtk_widget_get_gl_drawable(widget);
	glcontext  = gtk_widget_get_gl_context(widget);

	if (gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	{
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(3);
		glFinish();
		gdk_gl_drawable_gl_end(gldrawable);
	}
#endif
}

static void regen_loopuptable(ClutterActor * widget, IBusHandwriteEngine * engine)
{
	return ;
#if 0
	int i;
	MatchedChar *matched;
	char drawtext[32]={0};
	GtkWidget * bt;

	gtk_container_foreach(GTK_CONTAINER(widget),(GtkCallback)gtk_widget_destroy,0);

	int munber = ibus_handwrite_recog_getmatch(engine->engine,&matched,0);

	//画10个侯选字
	for (i = 0; i < MIN(munber,10) ; ++i)
	{
		sprintf(drawtext,"%d.%s",i,matched[i].chr);

		bt = gtk_button_new_with_label(drawtext);

		gtk_table_attach_defaults(GTK_TABLE(widget),bt,i%5,i%5+1,i/5,i/5+1);

		gtk_widget_show(bt);

		void clicked(GtkButton *button, IBusHandwriteEngine *engine)
		{
			ibus_handwrite_engine_commit_text(engine,GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button),"order")));
		}

		g_object_set_data(G_OBJECT(bt),"order",GINT_TO_POINTER(i));
		g_signal_connect(bt,"clicked",G_CALLBACK(clicked),engine);

		gtk_widget_show(bt);
	}
#endif
}


static gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event,
		gpointer user_data)
{
#if 0
	IBusHandwriteEngine * engine;

	engine = (IBusHandwriteEngine *) (user_data);

	GdkCursorType ct ;

	guint width,height;

	gtk_window_get_size(GTK_WINDOW(engine->drawpanel),&width,&height);


	ct = event->y < (height-50) ?  GDK_PENCIL:GDK_CENTER_PTR;

	if( event->state & (GDK_BUTTON2_MASK |GDK_BUTTON3_MASK ))
		ct = GDK_FLEUR;

	if(event->state & GDK_BUTTON2_MASK )
		ct = GDK_BOTTOM_RIGHT_CORNER;

	gdk_window_set_cursor(widget->window,gdk_cursor_new(ct));

	if (engine->mouse_state == GDK_BUTTON_PRESS) // 鼠标按下状态
	{
		gdk_window_set_cursor(widget->window,gdk_cursor_new(ct));

		engine->currentstroke.points
				= g_renew(GdkPoint,engine->currentstroke.points,engine->currentstroke.segments +1  );

		engine->currentstroke.points[engine->currentstroke.segments].x
				= event->x;
		engine->currentstroke.points[engine->currentstroke.segments].y
				= event->y;
		engine->currentstroke.segments++;
		printf("move, x= %lf, Y=%lf, segments = %d \n",event->x,event->y,engine->currentstroke.segments);

		gtk_widget_queue_draw(widget);
	    while (gtk_events_pending ())
	        gtk_main_iteration ();

	}
	else if(event->state & GDK_BUTTON2_MASK)
	{
		// change size
		width += event->x - engine->lastpoint.x;
		height += event->y - engine->lastpoint.y;

		gtk_window_resize(GTK_WINDOW(engine->drawpanel),width,height);

		g_debug("set size to %d,%d",width,height);

		widget_realize(engine->drawpanel,engine);

		engine->lastpoint.x = event->x;
		engine->lastpoint.y = event->y;
	}
	else if( event->state & (GDK_BUTTON2_MASK |GDK_BUTTON3_MASK ))
	{
		gtk_window_move(GTK_WINDOW(engine->drawpanel),event->x_root -engine->lastpoint.x,event->y_root - engine->lastpoint.y);
	}
#endif
	return FALSE;
}

static gboolean on_button(ClutterActor* widget, GdkEventButton *event, gpointer user_data)
{
	int i;
	IBusHandwriteEngine * engine;

	engine = (IBusHandwriteEngine *) (user_data);

	switch (event->type)
	{

	case GDK_BUTTON_PRESS:
		if(event->button != 1)
		{
			engine->mouse_state = 0;
			engine->lastpoint.x = event->x;
			engine->lastpoint.y = event->y;
			return TRUE;
		}

		engine->mouse_state = GDK_BUTTON_PRESS;

		g_print("mouse clicked\n");

		engine->currentstroke.segments = 1;

		engine->currentstroke.points = g_new(GdkPoint,1);

		engine->currentstroke.points[0].x = event->x;
		engine->currentstroke.points[0].y = event->y;

		break;
	case GDK_BUTTON_RELEASE:
		engine->mouse_state = GDK_BUTTON_RELEASE;

		ibus_handwrite_recog_append_stroke(engine->engine,engine->currentstroke);

		engine->currentstroke.segments = 0;
		g_free(engine->currentstroke.points);

		engine->currentstroke.points = NULL;

		ibus_handwrite_recog_domatch(engine->engine,10);

		g_print("mouse released\n");

		clutter_actor_queue_redraw(widget);
		regen_loopuptable(engine->lookuppanel,engine);

		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void UI_buildui(IBusHandwriteEngine * engine)
{
	//建立绘图窗口, 建立空点
	if (!engine->drawpanel)
	{
		engine->drawpanel = clutter_stage_new();
		clutter_stage_set_title(CLUTTER_STAGE(engine->drawpanel),"draw here");

		ClutterLayoutManager * box = clutter_box_layout_new();
		clutter_box_layout_set_vertical(CLUTTER_BOX_LAYOUT(box),1);

		clutter_layout_manager_set_container(box,CLUTTER_CONTAINER(engine->drawpanel));

//		clutter_container_add_actor(,box);

		ClutterActor * drawing_area = clutter_actor_new();

		clutter_actor_set_size(drawing_area,400,400);

		ClutterContent * drawer = clutter_canvas_new();
		clutter_canvas_set_size(CLUTTER_CANVAS(drawer),400,400);
		clutter_actor_set_content(drawing_area,drawer);

		g_signal_connect(G_OBJECT(drawer),"draw",G_CALLBACK(paint_lines_gl),engine);

		g_signal_connect(G_OBJECT(drawing_area),"motion-event",G_CALLBACK(on_mouse_move),engine);
		g_signal_connect(G_OBJECT(drawing_area),"button-release-event",G_CALLBACK(on_button),engine);
		g_signal_connect(G_OBJECT(drawing_area),"button-press-event",G_CALLBACK(on_button),engine);


		clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(box),
				drawing_area,1,1,1,CLUTTER_BOX_ALIGNMENT_START,CLUTTER_BOX_ALIGNMENT_CENTER);

		clutter_container_add_actor(CLUTTER_CONTAINER(engine->drawpanel),drawing_area);


//		engine->lookuppanel = clutter_table_layout_new();

//		clutter_box_layout_pack(CLUTTER_BOX_LAYOUT(box),				engine->lookuppanel,1,1,1,CLUTTER_BOX_ALIGNMENT_END,CLUTTER_BOX_ALIGNMENT_CENTER);


//		g_signal_connect(G_OBJECT(engine->drawpanel),"expose-event",G_CALLBACK(paint_lines_gl),engine);		}


/*
                gtk_widget_set_tooltip_markup(GTK_WIDGET(engine->drawpanel),
                                              _("<b>Hint:</b>\n"
                                                "Left mouse key to draw strokes.\n"
                                                "Holding right mouse key to move the widget.\n"
                                                ));

*/
//		gtk_window_set_position(GTK_WINDOW(engine->drawpanel),GTK_WIN_POS_MOUSE);

//		GtkWidget * vbox = gtk_vbox_new(FALSE,0);

//		gtk_container_add(GTK_CONTAINER(engine->drawpanel),vbox);

//		GtkWidget * drawing_area = gtk_drawing_area_new();

//		GdkGLConfig * glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_DOUBLE|GDK_GL_MODE_MULTISAMPLE);

//		if (gtk_widget_set_gl_capability(drawing_area, glconfig, NULL, FALSE,
//				GDK_GL_RGBA_TYPE))
/*		{
			g_signal_connect(G_OBJECT(drawing_area),"configure-event",G_CALLBACK(widget_resize),engine);
			g_signal_connect(G_OBJECT(drawing_area),"realize",G_CALLBACK(glwidget_realize),engine);
		else
		{
			//没有 GLX 就使用普通 GDK 绘图
			g_signal_connect(G_OBJECT(drawing_area),"expose-event",G_CALLBACK(paint_lines),engine);
		}

		gtk_box_pack_start(GTK_BOX(vbox),drawing_area,TRUE,TRUE,FALSE);

//		gtk_window_get_default_size(GTK_WINDOW(engine->drawpanel),200,250);

		gtk_widget_set_size_request(drawing_area,200,200);

		engine->lookuppanel = gtk_table_new(2,5,TRUE);

		gtk_box_pack_end(GTK_BOX(vbox),engine->lookuppanel,FALSE,TRUE,FALSE);
		gtk_widget_set_size_request(engine->lookuppanel,200,50);

		gtk_widget_add_events(GTK_WIDGET(drawing_area),GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK| GDK_BUTTON_PRESS_MASK);


		g_signal_connect(G_OBJECT(engine->drawpanel),"realize",G_CALLBACK(widget_realize),engine);
	*/}
	clutter_actor_show_all(engine->drawpanel);
}

void UI_show_ui(IBusHandwriteEngine * engine)
{
	GdkCursor* cursor;

	printf("%s \n", __func__);
	if (engine->drawpanel)
	{
		clutter_actor_show_all(engine->drawpanel);
	}
}

void UI_hide_ui(IBusHandwriteEngine * engine)
{
	if (engine->drawpanel)
	{
		clutter_actor_hide(engine->drawpanel);
	}
}

void UI_cancelui(IBusHandwriteEngine* engine)
{
	// 撤销绘图窗口，销毁点列表
	if (engine->drawpanel)
		clutter_actor_destroy(engine->drawpanel);
	engine->drawpanel = NULL;
}

static void widget_realize(ClutterActor *widget, gpointer user_data)
{
	return ;
#if 0
	GdkColor black, white;
	int R = 5;
	guint	width,height;

	//二值图像，白就是 1
	white.pixel = 1;
	black.pixel = 0;

	gtk_window_set_opacity(GTK_WINDOW(widget), 0.62);

	gtk_window_get_size(GTK_WINDOW(widget),&width,&height);

	pxmp = gdk_pixmap_new(NULL, width, height, 1);
	gc = gdk_gc_new(GDK_DRAWABLE(pxmp));

	gdk_gc_set_foreground(gc, &black);

	gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, 0, 0, width, height);

	gdk_gc_set_foreground(gc, &white);

	gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 0, 0, R*2, R*2, 0, 360 * 64);
	gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, width - R*2, 0, R*2, R*2, 0, 360
			* 64);
	gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, width - R*2, height - R*2, R*2, R*2, 0,
			360 * 64);
	gdk_draw_arc(GDK_DRAWABLE(pxmp), gc, 1, 0, height - R*2, R*2, R*2, 0, 360
			* 64);
	gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, 0, R, width, height - R*2);
	gdk_draw_rectangle(GDK_DRAWABLE(pxmp), gc, 1, R, 0, width - R*2, height);

	g_object_unref(gc);

	clutter_stage_set_user_resizable()

	gtk_widget_reset_shapes(widget);
	gtk_widget_shape_combine_mask(widget, pxmp, 0, 0);
	gtk_widget_input_shape_combine_mask(widget, pxmp, 0, 0);
	g_object_unref(pxmp);
#endif
}
