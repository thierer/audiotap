#include "wav2prg_api.h"

#include <string.h>
#include <malloc.h>

#include "loaders.h"

struct loader {
  const struct wav2prg_plugin_functions* functions;
  const char* name;
  struct loader *next;
};

static struct loader *loader_list = NULL;

static unsigned char register_loader(const struct wav2prg_plugin_functions* functions, const char* name) {
  struct loader *new_loader = malloc(sizeof(struct loader));
  struct loader **last_loader;
  const struct wav2prg_plugin_conf *model_conf;

  if (get_loader_by_name(name))
    return 0;/*duplicate name*/

  if(functions->recognize_block_as_mine_with_start_end_func
     && functions->get_block_info
    )
    return 0;

  if(
     (!functions->recognize_block_as_mine_with_start_end_func 
    || functions->recognize_block_as_mine_func)
   && !functions->get_block_info
    )
    return 0;

  model_conf = functions->get_new_plugin_state();
  if(
      (functions->recognize_block_as_mine_with_start_end_func
    || functions->recognize_block_as_mine_func)
    && !model_conf->dependency)
    return 0;

  for(last_loader = &loader_list; *last_loader != NULL; last_loader = &(*last_loader)->next);
  new_loader->functions=functions;
  new_loader->name=name;
  new_loader->next=NULL;
  *last_loader=new_loader;
  
  return 1;
}

static void unregister_first_loader(void) {
  struct loader *new_first_loader;
  
  if(loader_list == NULL)
    return;
  
  new_first_loader = loader_list->next;
  free(loader_list);
  loader_list = new_first_loader;
}

#if 1
const void turbotape_get_plugin(wav2prg_register_loader register_loader_func);
const void kernal_get_plugin(wav2prg_register_loader register_loader_func);
const void novaload_get_plugin(wav2prg_register_loader register_loader_func);
const void audiogenic_get_plugin(wav2prg_register_loader register_loader_func);
const void pavlodapenetrator_get_plugin(wav2prg_register_loader register_loader_func);
const void pavlodaold_get_plugin(wav2prg_register_loader register_loader_func);
const void pavloda_get_plugin(wav2prg_register_loader register_loader_func);
const void connection_get_plugin(wav2prg_register_loader register_loader_func);
const void rackit_get_plugin(wav2prg_register_loader register_loader_func);
#endif

void register_loaders(void) {
#if 1
  turbotape_get_plugin(register_loader);
  kernal_get_plugin(register_loader);
  novaload_get_plugin(register_loader);
  audiogenic_get_plugin(register_loader);
  pavlodapenetrator_get_plugin(register_loader);
  pavlodaold_get_plugin(register_loader);
  pavloda_get_plugin(register_loader);
  connection_get_plugin(register_loader);
  rackit_get_plugin(register_loader);
  turbocycle_get_plugin(register_loader);
#endif
}

const struct wav2prg_plugin_functions* get_loader_by_name(const char* name) {
  struct loader *loader;

  for(loader = loader_list; loader != NULL; loader = loader->next)
  {
    if(!strcmp(loader->name, name))
      return loader->functions;
  }
  return NULL;
}

char** get_loaders(unsigned char single_loader_analysis) {
  struct loader_for_single_loader_analysis {
    const char* name;
    struct loader_for_single_loader_analysis* next;
  } *valid_loaders = NULL, **loader_to_add = &valid_loaders, *this_loader;
  struct loader *loader;
  int found_loaders = 0, this_loader_index = 0;
  char** valid_loader_names;
  
  for(loader = loader_list; loader != NULL; loader = loader->next)
  {
    if(are_all_dependencies_ok(loader->name)
    && (!single_loader_analysis || loader->functions->get_block_info)) {
      *loader_to_add = malloc(sizeof(struct loader_for_single_loader_analysis));
      (*loader_to_add)->name = loader->name;
      (*loader_to_add)->next = NULL;
      loader_to_add = &(*loader_to_add)->next;
      found_loaders++;
    }
  }
  valid_loader_names = malloc(sizeof(char*) * (found_loaders + 1));
  for(this_loader = valid_loaders; this_loader != NULL; this_loader = this_loader->next)
    valid_loader_names[this_loader_index++] = strdup(this_loader->name);
  valid_loader_names[found_loaders] = NULL;
  return valid_loader_names;
}
