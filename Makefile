NAME = bit-crusher
LV2_BUNDLE = $(NAME).lv2
LIBS = `pkg-config --cflags --libs lv2-plugin`

all: lv2

lv2: clean-lv2
	mkdir $(LV2_BUNDLE)
	gcc -shared -fPIC -DPIC src/$(NAME).c src/lv2.c $(LIBS) -o $(LV2_BUNDLE)/$(NAME).so
	cp manifest.ttl $(NAME)-mono.ttl $(NAME)-stereo.ttl $(LV2_BUNDLE)

clean: clean-lv2

clean-lv2:
	rm -rf $(LV2_BUNDLE)
