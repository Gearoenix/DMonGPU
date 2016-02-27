#ifndef GEAROENIX_CORE_SHORTCUTS_HPP
#define GEAROENIX_CORE_SHORTCUTS_HPP
#ifdef glog
#error "Error: Logger shortcut (glog) is already defined."
#endif
#define glog(exp) gearoenix::core::Logger::err << "In file: " << __FILE__ << " in line: " << __LINE__ << "   " << exp << "\n"
#ifdef gter
#error "Error: Logger shortcut (gter) is already defined."
#endif
#define gter gearoenix::core::Logger::err.flush(); gearoenix::core::Logger::err.close(); std::exit(EXIT_FAILURE)
#ifdef gce
#error "Error: Logger shortcut (gce) is already defined."
#endif
#define gce(exp) glog(exp); gter
#endif